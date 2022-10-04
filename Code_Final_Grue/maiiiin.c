/*
 * Code_Final_Grue.c
 *
 * Created: 2022-03-15 15:23:42
 * Author : ar12310
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "utils.h"
#include "lcd.h"
#include "uart.h"
#include "driver.h"
#include <avr/interrupt.h>
/**
int main(void)
{
	char msg[17];
	char msg2[17];
	lcd_init();
	uart_init(UART_0);
	uart_init(UART_1);
	sei();
	
	//bool button_state;
	// Mettre la broche du bouton du joystick en entrée
	DDRA = clear_bit(DDRA, PA2);
	DDRA = clear_bit(DDRA, PA1);
	
	// Activer la "pull-up" interne de la broche pour forcer un état haut
	// quand le bouton n'est pas enfoncé
	PORTA = set_bit(PORTA, PA2);
	PORTA = set_bit(PORTA, PA1);
	
	// Mettre les bits 0,1,2,3,4,5 du port des DELs en sortie
	DDRB = set_bits(DDRB, 0b00000100);
	
	//Initialiser les différentes variables liées aux moteurs
	uint8_t x=0;
	uint8_t y=0;
	uint8_t p=0;
	uint8_t g=0;
	uint8_t a=0;
	uint8_t v;
	
	//Initialisation des entrées et des broches
	adc_init();
	pwm0_init();
	pwm1_init(20000);
	pwm1_set_PD4(20000);	//reset WIFI prevention
	pwm2_init();
	
	
    while (1) 
    {
		
		//Conditions Moteur en X
		if(uart_rx_buffer_nb_line(UART_0)){
			
			uart_get_line(UART_0, msg, 40);
			y = msg[0];
			x = msg[1];
			g = msg[2];
			p = msg[3];
			a = msg[4];
			
			if(x == 137){
				
				pwm0_set_PB4(0);
				sprintf(msg,"-> x: %d", x);
			}
			
			else if (x >= 0 && x < 135){
				v = -2*x + 255;
				pwm0_set_PB4(v);
				PORTB = set_bit(PORTB,PB2);
			}
			
			else if(x >= 140 && x <= 255){
				pwm0_set_PB4(x);
				PORTB = clear_bit(PORTB,PB2);
			}
		
		
		//Conditions Moteur en Y
		if(y == 140) {
			pwm0_set_PB3(0);
		}
		
		else if (y >= 0 && y < 130){
			v = -2*y + 255;
			pwm0_set_PB3(v);
			PORTB = set_bit(PORTB,PB1);
		}
		
		else if(y > 145 && y <= 255){
			pwm0_set_PB3(y);
			PORTB = clear_bit(PORTB,PB1);
		}
	
	
	//Conditions Moteur Glissière
		if(g > 50 && g < 205 ){
			pwm2_set_PD6(0);
		}
		
		else if (g > 205 && g <= 255){
			pwm2_set_PD6(g);
			PORTB = set_bit(PORTB,PB0);
		}
		
		else if(g >= 0 && g <= 50){
			v = -2*g + 255;
			pwm2_set_PD6(v);
			PORTB = clear_bit(PORTB,PB0);
		}
	
	
	//Conditions Pince
	if(p == 1){
		pwm1_set_PD5(2000);
	}
	
	else if (p == 0){
		pwm1_set_PD5(5000);
	}
	
		
	//Programme Automation
	/*
		if (a=1){
			pwm2_set_PD6(255);
			PORTB = set_bit(PORTB,PB0);
		}
		
	}
	
	//Affichage LCD Moteur x, y
	sprintf(msg,"x: %3d, y: %3d", x, y);
	lcd_set_cursor_position(0,0);
	lcd_write_string(msg);
	
	//Affichage LCD Glissière, Pince
	sprintf(msg2, "g: %3d, p: %3d", g, p);
	lcd_set_cursor_position(0,1);
	lcd_write_string(msg2);
	
	
	//DELAY
	//_delay_ms(20);
	}	
}
*/