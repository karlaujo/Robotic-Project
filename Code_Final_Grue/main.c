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

//Definir les constantes
#define HORAIRE 1
#define ANTIHORAIRE 0

//Ajouter les variables globales
volatile uint8_t clics=0;
volatile uint16_t degree;
volatile uint8_t dir=HORAIRE;
volatile uint16_t msec=0;
volatile uint8_t sec=0;
bool broche_state;

char str[40];
char str2[40];
char msg3[40];
char msg4[40];


// fct d'interruption sur INT0
ISR(INT0_vect) {

	if (read_bit(PIND, PD3)){
		dir=HORAIRE;
		clics++;
	}
	
	else {
		clics--;
		dir=ANTIHORAIRE;
	}

	if (clics == 255)
	clics = 23;
	
	else if (clics >= 24)
	clics = 0;
	
	degree=clics*360/24;
	
}

ISR (TIMER1_OVF_vect){
	msec++;
	if (msec>=1000){
		msec=0;
		sec++;
	}

}

int main(void)
{
	char msg[40];
	char msg2[40];
	char msg5[40];
	
	// Mettre la broche du bouton du joystick en entrée
	DDRD = clear_bit(DDRD, PD2);
	DDRD = clear_bit(DDRD, PD3);
	DDRA = clear_bit(DDRA, PA0);
	DDRA = clear_bit(DDRA, PA1);
	
	// On doit activer la "pull-up" interne de la broche pour PD3
	PORTD = set_bit(PORTD, PD2);
	PORTD = set_bit(PORTD, PD3);
	PORTA = set_bit(PORTA, PA0);
	PORTA = set_bit(PORTA, PA1);
	
	// Activer les interruptions
	EIMSK = set_bit(EIMSK, INT0);
	
	//Detecter les fronts montants
	EICRA = set_bit(EICRA, ISC01);
	EICRA = clear_bit(EICRA, ISC00);
	
	// Activation générale des interruptions
	sei();
	
	// Faire l'initialisation du LCD
	lcd_init();
	uart_init(UART_0);
	
	// Mettre les bits 0,1,2,3,4,5 du port des DELs en sortie
	DDRB = set_bits(DDRB, 0b00000100);
	
	//Initialiser les différentes variables liées aux moteurs
	uint8_t x=0;
	uint8_t y=0;
	uint8_t p=0;
	uint8_t g=0;
	uint8_t a=0;
	bool l1;
	bool l2;
	uint8_t v;
	
	//Initialisation des entrées et des broches
	adc_init();
	pwm0_init();
	pwm1_init(1000);
	pwm1_set_PD4(20000);	//reset WIFI prevention
	pwm2_init();
	
	uint8_t temps;
	x=137;
	y=140;
	g=100;
	
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
			pwm1_set_PD5(1000);
		}
		
		else if (p == 0){
			pwm1_set_PD5(5000);
		}
		
	//Conditions Limit Switch
		l1 = read_bit(PINA, PA0);
		l2 = read_bit(PINA, PA1);
		
		//Test batterie morte
		/*pwm0_set_PB3(0);
		pwm0_set_PB4(0);
		pwm2_set_PD6(0);
		a=1;
		*/
		
	//Programme Automation
		broche_state = read_bit(PINA, PA3);
		uint8_t val=0;
		
		if (a == 1){
			//Affichage LCD Automation
			lcd_clear_display();
			sprintf(msg3, "l1:%d,l2:%d,t:%d:%d",l1, l2, sec, msec);
			lcd_set_cursor_position(0,0);
			lcd_write_string(msg3);
			
			//Affichage LCD Angle et Direction
			sprintf(msg4, "Angle=%3d, Dir=%d", degree, dir);
			lcd_set_cursor_position(0,1);
			lcd_write_string(msg4);
			
			//Affichage TIME OVER
			if (sec > 120){
				lcd_clear_display();
				sprintf(msg3, "TIME", sec, msec);
				lcd_set_cursor_position(6,0);
				lcd_write_string(msg3);
				
				sprintf(msg4, "OVER!!", sec, msec);
				lcd_set_cursor_position(5,1);
				lcd_write_string(msg4);
				
				
				_delay_ms(2000);
				lcd_clear_display();
				lcd_set_cursor_position(15,1);
				_delay_ms(1000);
			}
			
			
			//Algorithme Automatique
			
				//quille #1
				if (degree >= 0 && degree < 10){
					//fleche
					if (sec >= 0 && sec < 2)
					pwm0_set_PB3(200);
					if (sec == 5)
					pwm0_set_PB3(0);
				}
				
				//quille #2
				if (degree >= 10 && degree <= 65){
					//chariot
					if (sec >= 3 && sec < 10)
					pwm0_set_PB4(200);
					if (sec == 7)
					pwm0_set_PB4(0);
					
					//fleche
					if (sec >= 10 && sec < 15)
					pwm0_set_PB3(200);
					if (sec == 15)
					pwm0_set_PB3(0);
				}
				
				//quille #3
				if (degree >= 65 && degree <= 120){
					//chariot
					if (sec >= 15 && sec < 22)
					pwm0_set_PB4(200);
					if (sec == 22)
					pwm0_set_PB4(0);
					PORTB = clear_bit(PORTB,PB2);
					
					//fleche
					if (sec >= 22 && sec < 25)
					pwm0_set_PB3(200);
					if (sec == 25)
					pwm0_set_PB3(0);
				}
				
				//quille #4
				if (degree >= 120 && degree <= 185){
					//chariot
					if (sec >= 25 && sec < 32 )
					pwm0_set_PB4(200);
					if (sec == 32)
					pwm0_set_PB4(0);
					PORTB = set_bit(PORTB,PB2);
					
					//fleche
					if (sec >= 32 && sec < 35)
					pwm0_set_PB3(200);
					if (sec == 35)
					pwm0_set_PB3(0);
				}
				
				//quille #5
				if (degree >= 185 && degree <= 245){
					//chariot
					if (sec >= 35 && sec < 42)
					pwm0_set_PB4(200);
					if (sec == 42)
					pwm0_set_PB4(0);
					PORTB = clear_bit(PORTB,PB2);
					
					//fleche
					if (sec >= 42 && sec < 45)
					pwm0_set_PB3(200);
					if (sec == 45)
					pwm0_set_PB3(0);
				}
				
				//quille #6
				if (degree >= 245 && degree <= 305){
					//chariot
					if (sec >= 45 && sec < 52 )
					pwm0_set_PB4(200);
					if (sec == 52)
					pwm0_set_PB4(0);
					PORTB = set_bit(PORTB,PB2);
					
					//fleche
					if (sec >= 52 && sec < 55)
					pwm0_set_PB3(200);
					if (sec == 55)
					pwm0_set_PB3(0);
				}
				
				//point final
				if (degree >= 305 && degree <= 330){
					//fleche
					if (sec >= 55 && sec < 58)
					pwm0_set_PB3(200);
					if (sec > 58)
					pwm0_set_PB3(0);
				}
		}
		
			else {
				msec=0;
				sec=0;
				//Affichage LCD Moteur x, y	
				lcd_clear_display();
				sprintf(str,"x: %3d, y: %3d", x, y);
				lcd_set_cursor_position(0,0);
				lcd_write_string(str);
			
				//Affichage LCD Glissière, Pince
				temps = temps + 1/100;
				sprintf(str2, "g: %3d, a: %d", g, degree);
				lcd_set_cursor_position(0,1);
				lcd_write_string(str2);
			}
	
		}	
	}
}

