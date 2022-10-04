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
	\file uart.c
	\brief Gère la communication bidirectionelle avec le port série
	\author Iouri Savard Colbert
	\date 13 Mars 2012 - Date de création
	\date 16 Septembre 2012 - Ajout du support de UART 1
	\date 24 janvier 2020 - Nettoyage pour le pojet
*/

/******************************************************************************
Includes and defines
******************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "fifo.h"


/******************************************************************************
Static variables
******************************************************************************/

static uint16_t baudrate_to_UBRR[] = {

#if F_CPU == 8000000UL     /*Fosc = 8.0000MHz*/
    207,    /* BAUDRATE_2400	Error : 0.2%  */
    103,    /* BAUDRATE_4800 	Error : 0.2%  */
    51,     /* BAUDRATE_9600 	Error : 0.2%  */
    25,     /* BAUDRATE_19200 */
    12,     /* BAUDRATE_38400 */
    8,      /* BAUDRATE_57600 */
    3,      /* BAUDRATE_115200 Error : 8.5% */
    1,      /* BAUDRATE_230400 */
    1,      /* BAUDRATE_250000 */

#elif F_CPU == 16000000UL     /*Fosc = 16.0000MHz*/
    416,    /* BAUDRATE_2400 */
    207,    /* BAUDRATE_4800 */
    103,    /* BAUDRATE_9600 */
    51,     /* BAUDRATE_19200 */
    25,     /* BAUDRATE_38400 */
    16,     /* BAUDRATE_57600 */
    8,      /* BAUDRATE_115200 */
    3,      /* BAUDRATE_230400 */
    3,      /* BAUDRATE_250000 */

#elif F_CPU == 20000000UL   /*Fosc = 20.0000MHz*/
    520,    /* BAUDRATE_2400 */
    259,    /* BAUDRATE_4800 */
    129,    /* BAUDRATE_9600 */
    64,     /* BAUDRATE_19200 */
    32,     /* BAUDRATE_38400 */
    21,     /* BAUDRATE_57600 */
    10,     /* BAUDRATE_115200 */
    4,      /* BAUDRATE_230400 */
    4,      /* BAUDRATE_250000 */

#else
    #error The UBRR values are not defined for your CPU frequency
#endif
};

static volatile uint8_t rx_buffer_0[UART_0_RX_BUFFER_SIZE];
static volatile uint8_t tx_buffer_0[UART_0_TX_BUFFER_SIZE];
static volatile uint8_t rx_buffer_1[UART_1_RX_BUFFER_SIZE];
static volatile uint8_t tx_buffer_1[UART_1_TX_BUFFER_SIZE];

static fifo_t rx_fifo_0;
static fifo_t tx_fifo_0;
static fifo_t rx_fifo_1;
static fifo_t tx_fifo_1;

static fifo_t* rx_fifo_list[] = {&rx_fifo_0, &rx_fifo_1};
static fifo_t* tx_fifo_list[] = {&tx_fifo_0, &tx_fifo_1};


/******************************************************************************
Static prototypes
******************************************************************************/

static void enable_UDRE_interupt(uart_e port);
static void disable_UDRE_interupt(uart_e port);

static void enable_RX_interupt(uart_e port);
static void disable_RX_interupt(uart_e port);


/******************************************************************************
Interupts
******************************************************************************/

/**
    \brief interupt quand le data register (UDRE) est prêt à recevoir d'autres
    données pour UART 0
*/
ISR(USART0_UDRE_vect){

    UDR0 = fifo_pop(&tx_fifo_0);

    if(fifo_is_empty(&tx_fifo_0) == TRUE){

        disable_UDRE_interupt(UART_0);
    }
}

/**
    \brief interupt quand le data register (UDR) a reçu une nouvelle donnée
    pour UART 0
*/
ISR(USART0_RX_vect){

    fifo_push(&rx_fifo_0, UDR0);
}


/**
    \brief interupt quand le data register (UDRE) est prêt à recevoir d'autres
    données pour UART 1
*/
ISR(USART1_UDRE_vect){

    UDR1 = fifo_pop(&tx_fifo_1);

    if(fifo_is_empty(&tx_fifo_1)){

        disable_UDRE_interupt(UART_1);
    }
}


/**
    \brief interupt quand le data register (UDR) a reçu une nouvelle donnée
    pour UART 1
*/
ISR(USART1_RX_vect){

    fifo_push(&rx_fifo_1, UDR1);
}

/******************************************************************************
Global functions
******************************************************************************/
/*** initialize uart ***/
void uart_init(uart_e port){

    switch(port){
    case UART_0:

        /* configure asynchronous operation, no parity, 1 stop bit, 8 data bits,  */
        UCSR0C = (	(0 << UMSEL01) | /*USART Mode Select : Asynchronous USART*/
                    (0 << UMSEL00) | /*USART Mode Select : Asynchronous USART*/
                    (0 << UPM01) |	 /*Parity Mode : No parity*/
                    (0 << UPM00) |   /*Parity Mode : No parity*/
                    (0 << USBS0) |	 /*Stop Bit Select : 1-bit*/
                    (1 << UCSZ01) |  /*Character Size : 8-bit*/
                    (1 << UCSZ00) |  /*Character Size : 8-bit*/
                    (0 << UCPOL0));  /*0 when asynchronous mode is used*/

        /* enable RxD/TxD and ints */
        UCSR0B = (  (1 << RXCIE0) |  /*RX Complete Interrupt Enable*/
                    (0 << TXCIE0) |  /*TX Complete Interrupt Enable */
                    (0 << UDRIE0) |  /*Data Register Empty Interrupt Enable */
                    (1 << RXEN0) |   /*Receiver Enable*/
                    (1 << TXEN0) |   /*Transmitter Enable*/
                    (0 << UCSZ02));  /*Character Size : 8-bit*/

        UCSR0A = (  (0 << U2X0) |    /*Double the USART Transmission Speed*/
                    (0 << MPCM0));   /*Multi-processor Communication Mode*/

        /*initialisation des fifos respectifs */
        fifo_init(&rx_fifo_0, (uint8_t*)rx_buffer_0, UART_0_RX_BUFFER_SIZE);
        fifo_init(&tx_fifo_0, (uint8_t*)tx_buffer_0, UART_0_TX_BUFFER_SIZE);

        break;


    case UART_1:
        /* configure asynchronous operation, no parity, 1 stop bit, 8 data bits,  */
        UCSR1C = (	(0 << UMSEL01) | /*USART Mode Select : Asynchronous USART*/
                    (0 << UMSEL00) | /*USART Mode Select : Asynchronous USART*/
                    (0 << UPM01) |	 /*Parity Mode : No parity*/
                    (0 << UPM00) |   /*Parity Mode : No parity*/
                    (0 << USBS0) |	 /*Stop Bit Select : 1-bit*/
                    (1 << UCSZ01) |  /*Character Size : 8-bit*/
                    (1 << UCSZ00) |  /*Character Size : 8-bit*/
                    (0 << UCPOL0));  /*0 when asynchronous mode is used*/

        UCSR1B = (  (1 << RXCIE0) |  /*RX Complete Interrupt Enable*/
                    (0 << TXCIE0) |  /*TX Complete Interrupt Enable */
                    (0 << UDRIE0) |  /*Data Register Empty Interrupt Enable */
                    (1 << RXEN0) |   /*Receiver Enable*/
                    (1 << TXEN0) |   /*Transmitter Enable*/
                    (0 << UCSZ02));  /*Character Size : 8-bit*/

        UCSR1A = (  (0 << U2X0) |    /*Double the USART Transmission Speed*/
                    (0 << MPCM0));   /*Multi-processor Communication Mode*/

        /*initialisation des fifos respectifs */
        fifo_init(&rx_fifo_1, (uint8_t*)rx_buffer_1, UART_1_RX_BUFFER_SIZE);
        fifo_init(&tx_fifo_1, (uint8_t*)tx_buffer_1, UART_1_TX_BUFFER_SIZE);


        break;
    }

    uart_set_baudrate(port, DEFAULT_BAUDRATE);
}


/*** uart_set_baudrate ***/
/// \todo (iouri#1#): implémenter qqch qui empêche la corruption de la transmission.  La mise à jour de UBRR est immédiate.  Voir doc p. 196
void uart_set_baudrate(uart_e port, baudrate_e baudrate){

    switch(port){
    case UART_0:

        UBRR0 = baudrate_to_UBRR[baudrate];
        break;

    case UART_1:

        UBRR1 = baudrate_to_UBRR[baudrate];
        break;
    }
}



/*** uart_put_byte ***/
void uart_put_byte(uart_e port, uint8_t byte){

    //on commence par désactiver l'interuption pour éviter que celle-ci
    //se produise pendant qu'on ajoute un caractère au buffer
    disable_UDRE_interupt(port);

    fifo_push(tx_fifo_list[port], byte);

    // On active l'interrupt après avoir incrémenté le pointeur
    // d'entré pour éviter un dead lock assez casse-tête
    enable_UDRE_interupt(port);

}


/*** uart_put_string ***/
void uart_put_string(uart_e port, char* string){

	uint8_t i = 0;

	while(string[i] != '\0'){

		// On attend à l'infini qu'il y ait de la place dans le buffer. Je ne me
		// souviens pas d'avoir écrit ça, et je ne trouve pas ça du très beau code.
		//TODO évaluer la pertinance
		while(fifo_is_full(tx_fifo_list[port])  == TRUE);

		//on commence par désactiver l'interuption pour éviter que celle-ci
		//se produise pendant qu'on ajoute un caractère au buffer
		disable_UDRE_interupt(port);

		while((string[i] != '\0') && (fifo_is_full(tx_fifo_list[port])  == FALSE)){

			fifo_push(tx_fifo_list[port], string[i]);

			i++;
		}

		// On active l'interrupt après avoir incrémenté le pointeur
		// d'entré pour éviter un dead lock assez casse-tête
		enable_UDRE_interupt(port);

	}
}

/*** uart_get_byte ***/
uint8_t uart_get_byte(uart_e port){

    uint8_t byte;

    disable_RX_interupt(port);

    byte = fifo_pop(rx_fifo_list[port]);

    enable_RX_interupt(port);

    return byte;
}


void uart_get_string(uart_e port, char* out_buffer, uint8_t buffer_length){

	uint8_t index = 0;

	while(uart_is_rx_buffer_empty(port) == FALSE){

		out_buffer[index] = uart_get_byte(port);
		index++;

		// Si il ne reste de la place que pour le \0
		if(index >= buffer_length - 1){

			// On sort
			break;
		}
	}


	// Très temporaire pour déboguer
	if(index == 0){

		string_copy(out_buffer, "-vide-");
	}

	// On ferme la string
	out_buffer[index] = '\0';
}


/*** uart_clean_rx_buffer ***/
void uart_clean_rx_buffer(uart_e port){

	fifo_clean(rx_fifo_list[port]);
}

/*** uart_flush ***/
void uart_flush(uart_e port){

	while(uart_is_tx_buffer_empty(port) == FALSE);
}

/*** is_rx_buffer_empty ***/
bool uart_is_rx_buffer_empty(uart_e port){

    return fifo_is_empty(rx_fifo_list[port]);
}

/*** is_tx_buffer_empty ***/
bool uart_is_tx_buffer_empty(uart_e port){

    return fifo_is_empty(tx_fifo_list[port]);
}


/******************************************************************************
Static functions
******************************************************************************/

static void enable_UDRE_interupt(uart_e port){

    switch(port){
    case UART_0:

        UCSR0B = set_bit(UCSR0B, UDRIE0);
        break;

    case UART_1:

        UCSR1B = set_bit(UCSR1B, UDRIE1);
        break;
    }
}

static void disable_UDRE_interupt(uart_e port){

    switch(port){
    case UART_0:

        UCSR0B = clear_bit(UCSR0B, UDRIE0);
        break;

    case UART_1:

        UCSR1B = clear_bit(UCSR1B, UDRIE1);
        break;
    }
}

static void enable_RX_interupt(uart_e port){

    switch(port){
    case UART_0:

        UCSR0B = set_bit(UCSR0B, RXCIE0);
        break;

    case UART_1:

        UCSR1B = set_bit(UCSR1B, RXCIE1);
        break;
    }
}

static void disable_RX_interupt(uart_e port){

    switch(port){
    case UART_0:

        UCSR0B = clear_bit(UCSR0B, RXCIE0);
        break;

    case UART_1:

        UCSR1B = clear_bit(UCSR1B, RXCIE1);
        break;
    }

}
