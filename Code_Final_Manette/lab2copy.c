/*
 * GccApplication1.c
 *
 * Created: 2022-02-07 14:48:05
 * Author : AR72760
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "driver.h"
#include "lcd.h"
#include "utils.h"

void heartbeat(void);

int main(void)
{
	uint8_t x;
	uint8_t y;
	char str[40];
	adc_init();
	lcd_init();
	
	//Controle du joystick
	bool button_state;
	DDRA = clear_bit(DDRA, PA2); // Mettre la broche du bouton du joystick en entrée
	PORTA = set_bit(PORTA, PA2);
	
	//Controle du servomoteur
	uint16_t top=19999;
	pwm1_init(top);
	
	//Configuration Joystick
	DDRA = set_bit(DDRA, PA2);
	PORTA = set_bit(PORTA,PA2);
    
    while (1) 
    {	
		//Lecture du potentiometre
		x = adc_read(0);
		y = adc_read(1);
		lcd_set_cursor_position(0,0);
		sprintf(str, "X=%d, Y=%d", x, y);
		lcd_write_string(str);
		
		
		//Controle du joystick
		button_state = read_bit(PINA, PA2);
		pwm1_set_PD4(1000);
		
		if(button_state == TRUE) {
		pwm1_set_PD4(2000);
		}
		
		
		//heartbeat
		heartbeat();//témoin alphabétique
		_delay_ms(100);
    }
}

void heartbeat(void){
	static uint8_t c='A';
	lcd_set_cursor_position(0,1);
	lcd_write_char(c);
	c=((c>='Z')?'A':c+1);
}