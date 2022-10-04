/*
	 __ ___  __
	|_   |  (_
	|__  |  __)

	MIT License

	Copyright (c) 2018	École de technologie supérieure

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify and/or merge copies of the Software, and to permit persons
	to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.
*/
/**
	\file lcd.c
	\brief driver pour un affichage LCD piloté par un HD44780
	\author Iouri Savard Colbert
	\date 28 avril 2014
	\date Modifié le 13 juillet 2019

*/

/******************************************************************************
Includes
******************************************************************************/

#include <avr/io.h>
#include <util/delay_basic.h>  //Pour une raison obsucre <util/delay.h> boguais
#include "lcd.h"


/******************************************************************************
Defines
******************************************************************************/

#define FUNCTION_SET_4_BITS 0b00101000
#define FUNCTION_SET_8_BITS 0b00111000

#if defined(HD44780_BUS_4_BITS)
    #define BUS_SHIFT           2
    #define BUS_MASK            (0b11110000 >> BUS_SHIFT)
    #define FUNCTION_SET        FUNCTION_SET_4_BITS
#elif defined(HD44780_BUS_8_BITS)
    #define BUS_SHIFT           0
    #define BUS_MASK            0b11111111
    #define FUNCTION_SET        FUNCTION_SET_8_BITS
#else
    #error Le bus du HD44780 doit être de 4 ou 8 bits (voir lcd.h)
#endif

#define FALLING_EDGE()  CTRL_PORT = clear_bit(CTRL_PORT, E_PIN)
#define RISING_EDGE()   CTRL_PORT = set_bit(CTRL_PORT, E_PIN)
#define COMMAND_MODE()  CTRL_PORT = clear_bit(CTRL_PORT, RS_PIN)
#define DATA_MODE()     CTRL_PORT = set_bit(CTRL_PORT, RS_PIN)
#define WRITE()         CTRL_PORT = clear_bit(CTRL_PORT, RS_PIN)
#define READ()          CTRL_PORT = set_bit(CTRL_PORT, RS_PIN)

#define MAX_INDEX (LCD_NB_ROW * LCD_NB_COL)

#define BLANK_CHAR (' ')


/******************************************************************************
Static variables
******************************************************************************/

/* LCD */
static uint8_t local_index;
static bool clear_required_flag;


/******************************************************************************
Static prototypes
******************************************************************************/

/* hd44780 */
static void clock_data(char data);


/* lcd */
bool shift_local_index(bool foward);
uint8_t index_to_col(uint8_t index);
uint8_t index_to_row(uint8_t index);


/******************************************************************************
Global functions HD44780
******************************************************************************/

void hd44780_init(bool increment, bool cursor, bool blink){

    //On définie la valeur par défaut des ports
    CTRL_PORT = clear_bit(CTRL_PORT, RS_PIN);   //command mode
    CTRL_PORT = clear_bit(CTRL_PORT, RW_PIN);   //write mode

	// On change la direction des ports
    DATA_DDR = BUS_MASK;
    CTRL_DDR = set_bits(CTRL_DDR, (1 << E_PIN) | (1 << RW_PIN) | (1 << RS_PIN));


    DATA_PORT = (0b00110000 >> BUS_SHIFT) & BUS_MASK; //Function set (Interface is 8 bits long)
    _delay_loop_2(0xFFFF);     //13.1ms (idéalement 15ms, mais pas possible)
    FALLING_EDGE();
    //_delay_loop_2(20000);    //4ms
    _delay_loop_2(0xFFFF);     //13.1ms (idéalement 15ms, mais pas possible)
    RISING_EDGE();

    DATA_PORT = (0b00110000 >> BUS_SHIFT) & BUS_MASK; //Function set (Interface is 8 bits long)
    //_delay_loop_2(20500);    //4.1ms
    _delay_loop_2(0xFFFF);     //13.1ms (idéalement 15ms, mais pas possible)
    FALLING_EDGE();
    //_delay_loop_2(20000);    //4ms
    _delay_loop_2(0xFFFF);     //13.1ms (idéalement 15ms, mais pas possible)
    RISING_EDGE();

    DATA_PORT = (0b00110000 >> BUS_SHIFT) & BUS_MASK; //Function set (Interface is 8 bits long)
    _delay_loop_2(500);     //100us
    FALLING_EDGE();
    _delay_loop_2(500);     //100us
    RISING_EDGE();

    DATA_PORT = (FUNCTION_SET >> BUS_SHIFT) & BUS_MASK;
    _delay_loop_2(500);     //100us
    FALLING_EDGE();
    _delay_loop_2(500);     //100us
    RISING_EDGE();

    clock_data(FUNCTION_SET);

    hd44780_set_entry_mode(increment);
    hd44780_set_display_control(TRUE, cursor, blink);
    hd44780_clear_display();
}


void hd44780_clear_display(){

    COMMAND_MODE();

    clock_data(0b00000001);     //Clear Display

	// Cette information n'est nulle part dans la datasheet, mais a plutôt été trouvée
	// par essai erreur. Une bonne solution pour régler le problème sera de relire le busy
	// flag
	//TODO
	//_delay_ms(2);
	_delay_loop_2(10000);

    DATA_MODE();
}


void hd44780_set_entry_mode(bool increment){

    uint8_t increment_decrement;

    if(increment == TRUE){

        increment_decrement = 0b00000010;
    }

	else{

        increment_decrement = 0b00000000;
    }

    COMMAND_MODE();

    clock_data(0b00000100 | increment_decrement);     //Entry mode set

    DATA_MODE();
}


void hd44780_set_display_control(bool display, bool cursor, bool blink){

    uint8_t dcb = 0;

    if(display == TRUE){

        dcb = set_bit(dcb, 2);
    }

    if(cursor == TRUE){

        dcb = set_bit(dcb, 1);
    }

    if(blink == TRUE){

        dcb = set_bit(dcb, 0);
    }

    COMMAND_MODE();

    clock_data(0b00001000 | dcb);     //Display on/off control

    DATA_MODE();
}


void hd44780_set_cursor_position(uint8_t col, uint8_t row){

    uint8_t address = 0;

    //On commence par ajouter le offset de la ligne
    switch(row){
    case 0:

        address += 0x00;
        break;

    case 1:

        address += 0x40;
        break;
    }

    //Puis on ajoute le offset de la colone
    address += col;

    COMMAND_MODE();

    clock_data(0b10000000 | address);     //Set DDRAM address

    DATA_MODE();
}


void hd44780_shift_cursor(hd44780_shift_e shift){

    uint8_t right_left;

    if(shift == HD44780_SHIFT_RIGHT){

        right_left = 0b00000100;
    }

    else{

        right_left = 0b00000000;
    }

    COMMAND_MODE();

    clock_data(0b00010000 | right_left);     //Cursor or display shift

    DATA_MODE();
}


void hd44780_write_char(unsigned char character){

#ifdef ENABLE_JAPANESE_CHAR
	const char MAX_CHAR = 255;
#else
	const char MAX_CHAR = CHAR_LEFT_ARROW;
#endif

    DATA_MODE();

    if((character >= 0) && (character <= MAX_CHAR)){

            clock_data(character);
    }

    else{

		switch(character){
		case 0xC0:	//À
		case 0xC1:	//A accent aigue
		case 0xC2:	//Â
		case 0xC3:	//A ???
		case 0xC4:	//Ä
			character = 'A';
			break;

		case 0xC7:	//Ç
			character = 'C';
			break;

		case 0xC8:	//È
		case 0xC9:	//É
		case 0xCA:	//Ê
		case 0xCB:	//Ë
			character = 'E';
			break;

		case 0xCC:	//Ì
		case 0xCD:	//I accent aigue
		case 0xCE:	//Î
		case 0xCF:	//Ï
			character = 'I';
			break;

		case 0xD2:	//Ò
		case 0xD3:	//O accent aigue
		case 0xD4:	//Ô
		case 0xD5:	//O ???
		case 0xD6:	//Ö
			character = 'O';
			break;

		case 0xD9:	//Ù
		case 0xDA:	//U accent aigue
		case 0xDB:	//Û
		case 0xDC:	//Ü
			character = 'U';
			break;

		case 0xE0:	//à
		case 0xE1:	//a accent aigue
		case 0xE2:	//â
		case 0xE3:	//a ???
		case 0xE4:	//ä
			character = 'a';
			break;

		case 0xE7:	//ç
			character = 'c';
			break;

		case 0xE8:	//è
		case 0xE9:	//é
		case 0xEA:	//ê
		case 0xEB:	//ë
			character = 'e';
			break;

		case 0xEC:	//ì
		case 0xED:	//i accent aigue
		case 0xEE:	//î
		case 0xEF:	//ï
			character = 'i';
			break;

		case 0xF2:	//ò
		case 0xF3:	//o accent aigue
		case 0xF4:	//ô
		case 0xF5:	//o ???
		case 0xF6:	//ö
			character = 'o';
			break;

		case 0xF9:	//ù
		case 0xFA:	//u accent aigue
		case 0xFB:	//û
		case 0xFC:	//ü
			character = 'u';
			break;


		default:
			//character = 0b10100101;  //une boule pas rapport
			character = '?';  //une boule pas rapport
			break;
		}

        clock_data(character);  //une boule pas rapport
    }
}

void hd44780_write_cgram(uint8_t slot, const uint8_t* bitmap_array){

    //Vérification que slot est inclusivement entre 0 et 7 + décalage
    slot = (slot & 0b00000111) << 3;

    COMMAND_MODE();

    clock_data(0b01000000 | slot);     //Set CGRAM address

    DATA_MODE();

    for(uint8_t i = 0; i < 8; i++){

        clock_data(bitmap_array[i]);
    }

    hd44780_set_cursor_position(0,0);

    DATA_MODE();

//    clock_data(0);

}


/******************************************************************************
Global functions LCD
******************************************************************************/

void lcd_init(void){

    hd44780_init(TRUE, TRUE, FALSE);

    local_index = 0;
	clear_required_flag = FALSE;
}


void lcd_clear_display(){

    hd44780_clear_display();

    local_index = 0;
}


void lcd_set_cursor_position(uint8_t col, uint8_t row){

    if((col >= 0) && (col < LCD_NB_COL) && (row >= 0) && (row < LCD_NB_ROW)){

        hd44780_set_cursor_position(col, row);

        local_index = col + row * LCD_NB_COL;
    }
}


void lcd_shift_cursor(lcd_shift_e shift){

    switch(shift){
    case LCD_SHIFT_RIGHT:

        shift_local_index(TRUE);

        break;

    case LCD_SHIFT_LEFT:

        shift_local_index(FALSE);

        break;

    case LCD_SHIFT_UP:

		// Si on est sur la ligne du haut
        if(index_to_row(local_index) <= 0){

			// On se rend à la fin moins une ligne
            local_index += (MAX_INDEX - LCD_NB_COL);

			clear_required_flag = TRUE;
        }

        else{

			// On recule d'une ligne
            local_index -= LCD_NB_COL;
        }

        break;

    case LCD_SHIFT_DOWN:

		// Si on est rendu à la dernière ligne
        if(index_to_row(local_index) >= LCD_NB_ROW - 1){

			// On ne garde que le numéro de colone (donc sa ramène sur la première ligne)
            local_index %= LCD_NB_COL;

			clear_required_flag = TRUE;
        }

        else{

			// On avance d'une ligne
            local_index += LCD_NB_COL;
        }

        break;

	case LCD_SHIFT_END:

		local_index = ((index_to_row(local_index) + 1) * LCD_NB_COL) - 1;

		break;

	case LCD_SHIFT_START:

		local_index = index_to_row(local_index) * LCD_NB_COL;

		break;

	case LCD_SHIFT_TOP:

		// On ne garde que le numéro de colone (donc sa ramène sur la première ligne)
		local_index %= LCD_NB_COL;

		break;

	case LCD_SHIFT_BOTTOM:

		// On se rend à la fin moins une ligne
		local_index += (MAX_INDEX - LCD_NB_COL);

		break;
	}

    hd44780_set_cursor_position(index_to_col(local_index), index_to_row(local_index));
}


void lcd_write_char(char character){

    bool unsynced;

	// Si il s'agit d'un des 32 premier caractères ascii, on s'attend à un contrôle
	// plutôt que l'affichage d'un caractère
	if(character < ' '){

		switch (character){
		case '\n':	// 0x0A	new line
			lcd_shift_cursor(LCD_SHIFT_DOWN);
			break;
		case '\r':
			lcd_shift_cursor(LCD_SHIFT_START);
			break;
		}
	}

	else{

		if(clear_required_flag == TRUE){

			hd44780_clear_display();
			//hd44780_set_cursor_position(index_to_col(local_index), index_to_row(local_index));
			clear_required_flag = FALSE;
		}

		hd44780_write_char(character);

		unsynced = shift_local_index(TRUE);

		if(unsynced == TRUE){

			hd44780_set_cursor_position(index_to_col(local_index), index_to_row(local_index));
		}
	}
}


void lcd_write_string(const char* string){

    uint8_t index = 0;

    while(string[index] != '\0'){

        lcd_write_char(string[index]);

        index++;
    }
}


/******************************************************************************
Static functions
******************************************************************************/

/* hd44780 */
void clock_data(char data){

    DATA_PORT = (data >> BUS_SHIFT) & BUS_MASK;

    //_delay_us(50);
    //_delay_loop_2(250);
    _delay_loop_2(500);

    FALLING_EDGE();

    //_delay_us(50);
    //_delay_loop_2(250);
    _delay_loop_2(500);

    RISING_EDGE();

#ifdef HD44780_BUS_4_BITS
    DATA_PORT = (data << (4 - BUS_SHIFT)) & BUS_MASK;

    //_delay_us(50);
    //_delay_loop_2(250);
    _delay_loop_2(500);

    FALLING_EDGE();

    //_delay_us(50);
    //_delay_loop_2(250);
    _delay_loop_2(500);

    RISING_EDGE();
#endif // HD44780_BUS_4_BITS
}


/* lcd */

uint8_t index_to_col(uint8_t index){

    return index % LCD_NB_COL;
}


uint8_t index_to_row(uint8_t index){

    return index / LCD_NB_COL;
}

bool shift_local_index(bool foward){

    uint8_t previous_row;

    previous_row = index_to_row(local_index);

    /* Si on est dans le sens foward */
    if(foward == TRUE){

        /* Si on est à la fin */
        if(local_index >= MAX_INDEX - 1){

			local_index = 0;
        }

        else{

            local_index++;
        }
    }

    /* Si on est dans le sens décrémental foward ou incrémental backward*/
    else{

        /* Si on est au début */
        if(local_index <= 0){

            local_index = MAX_INDEX - 1;
        }

        else{

            local_index--;
        }
    }

    /* Si la row actuelle ne correspond pas à l'ancienne il va falloir manuellement
    déplacer le curseur */
    return (previous_row != index_to_row(local_index));
}
