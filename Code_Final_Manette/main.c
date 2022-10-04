/*
 * Code_Final_Manette.c
 *
 * Created: 2022-03-28 14:09:43
 * Author : AR72760
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "driver.h"
#include "lcd.h"
#include "utils.h"
#include "uart.h"
#include <avr/interrupt.h>

//Timer
#include <time.h>     //For clock(),clock_t

int main(void)
{
	
	DDRA = clear_bit(DDRA, PA2); // Mettre la broche du bouton du joystick en entrée
	PORTA = set_bit(PORTA, PA2);
	
	DDRA = clear_bit(DDRA, PA1); // Mettre le moteur en x en entrée
	PORTA = set_bit(PORTA, PA1);
	
	DDRA = clear_bit(DDRA, PA0); // Mettre le moteur en y en entrée
	PORTA = set_bit(PORTA, PA0);
	
	DDRD = clear_bit(DDRD, PD5); // Mettre l'actionneur d'automation en entrée
	PORTD = set_bit(PORTD, PD5);
	
	
	DDRD = clear_bit(DDRD, PD7); // Mettre le stoppeur d'automation en entrée
	PORTD = set_bit(PORTD, PD7);
	
	
	lcd_init();
	uart_init(UART_0);
	sei();
	char str[40];
	char str2[40];
	adc_init();
	uint8_t t = 0;
	
	while (1)
	{
		
		//Moteur en x (chariot)
		uint8_t y = adc_read(PA1);
		uart_put_byte(UART_0, y);
		if (y == 10)
		y = 11;
		
		//Moteur en y (Tourner la fleche)
		uint8_t x = adc_read(PA0);
		uart_put_byte(UART_0, x);
		if(x == 10)
		x = 11;
		
		//Moteur Glissiere
		uint8_t g = adc_read(PA3);
		uart_put_byte(UART_0, g);
		if (g == 10)
		g = 11;
		
		//Servomoteur pour la Pince
		uint8_t p = read_bit(PINA, PA2);
		uart_put_byte(UART_0, p);
		
		//Temps
		t = t + 12;
		
		//Programme automation
		bool auto_start = read_bit(PIND, PD5);
		bool auto_stop = read_bit(PIND, PD7);
		uint8_t a_start;
		uint8_t a_stop;
		char mode[40];
		
		if (auto_start == FALSE) {
			a_start = 1;
			a_stop = 0;
			//Affichage
			lcd_clear_display();
			sprintf(mode, "mode auto");
			lcd_set_cursor_position(7,1);
			lcd_write_string(mode);
			
			if (auto_stop == FALSE){
				a_start = 0;
				a_stop = 1;
				//Affichage
				lcd_clear_display();
				sprintf(mode, "mode man");
				lcd_set_cursor_position(7,1);
				lcd_write_string(mode);
			}
		}
		
		if (auto_stop == FALSE){
			a_start = 0;
			a_stop=1;
			//Affichage
			lcd_clear_display();
			sprintf(mode, "mode man");
			lcd_set_cursor_position(7,1);
			lcd_write_string(mode);
		}
		
		uart_put_byte(UART_0, a_start);
		uart_put_byte(UART_0, '\n');
		
		
		//Affichage LCD Moteur x, y
		sprintf(str,"x: %3d, y: %3d", x, y);
		lcd_set_cursor_position(0,0);
		lcd_write_string(str);
		
		//Affichage LCD Glissiere, Pince
		sprintf(str2, "g: %3d", g);
		lcd_set_cursor_position(0,1);
		lcd_write_string(str2);
		lcd_set_cursor_position(15,1);
		//DELAY
		_delay_ms(100);
	}
}
