#ifndef UART_H_INCLUDED
#define UART_H_INCLUDED

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
	\file uart.h
	\brief Gère la communication bidirectionelle avec le port série
	\author Iouri Savard Colbert
	\date 13 Mars 2012 - Date de création
	\date 16 Septembre 2012 - Ajout du support de UART 1
	\date 24 janvier 2020 - Nettoyage pour le pojet
*/

/******************************************************************************
Includes
******************************************************************************/

#include "utils.h"

/******************************************************************************
Defines
******************************************************************************/

#define UART_0_RX_BUFFER_SIZE 150
#define UART_0_TX_BUFFER_SIZE 150

#define UART_1_RX_BUFFER_SIZE 16
#define UART_1_TX_BUFFER_SIZE 16

typedef enum{

    UART_0 = 0,
    UART_1

}uart_e;


typedef enum{

    BAUDRATE_2400 = 0,
    BAUDRATE_4800,
    BAUDRATE_9600,
    BAUDRATE_19200,
    BAUDRATE_38400,
    BAUDRATE_57600,
    BAUDRATE_115200,
    BAUDRATE_230400,
    BAUDRATE_250000,

}baudrate_e;

#define DEFAULT_BAUDRATE BAUDRATE_9600

/******************************************************************************
Prototypes
******************************************************************************/

/**
    \brief Fait l'initialisation du UART
	\param port Le numéro du port du microcontrôleur (UART_0 ou UART_1)
*/
void uart_init(uart_e port);


/**
    \brief Définit le badrate du port choisit
	\param port Le numéro du port du microcontrôleur (UART_0 ou UART_1)
*/
void uart_set_baudrate(uart_e port, baudrate_e baudrate);


/**
    \brief Ajoute un byte au rolling buffer à envoyer par le UART
	\param port Le numéro du port du microcontrôleur (UART_0 ou UART_1)
    \param byte le byte à ajouter
*/
void uart_put_byte(uart_e port, uint8_t byte);


/**
    \brief Ajoute la string (par copie) au rolling buffer à envoyer par le UART.
	\param port Le numéro du port du microcontrôleur (UART_0 ou UART_1)
    \param un pointeur sur le premier char de la string

	La copie s'arrête au premier \0. Ce dernier n'est pas copié. Si la string est plus
	longue que l'espace qui est libre dans le buffer, la fonction va patiement attendre
	que de l'espace se libère. Dans cette situation, cette fonction peut être très longue
	à retourner, ce qui pourrait briser des timmings critiques dans le code.
*/
void uart_put_string(uart_e port, char* string);

/**
    \brief Retire un byte au rolling buffer reçu par le UART.
	\param port Le numéro du port du microcontrôleur (UART_0 ou UART_1)
    \return le byte reçu

    Si le buffer est vide cette fonction retourne '\0'.  Si le caractère à retourner
    est '\0' la fonction retourne aussi cette valeur.  Pour faire la différence entre
    les deux il faut préalablement vérifier si le buffer est vide avec la fonction
    uart_is_rx_buffer_empty()
*/
uint8_t uart_get_byte(uart_e port);

/**
    \brief Retourne tout ce que que le buffer de réception contient sous la forme d'une string
	\param port Le numéro du port du microcontrôleur (UART_0 ou UART_1)

    Pour que cette fonction puisse fonctionner le buffer qui lui est passé doit minimalement
	avoir deux bytes de long. Un byte pour retourner au moins un caractère et un autre pour
	le \0. Si le buffer est plus petit que ça, cette fonction va corrompre la mémoire sans
	prévenir.

	Dès que le buffer de réception est vide, la fonction termine la string avec un \0 et
	retourne. Comme le UART est relativement très lent par rapport au processeur, il faut
	attendre un peu avant d'appeler cette fonction.

	Si le buffer est vide au moment d'appeler cette fonction, celle-ci retourne une string
	d'un seul caractère; \0

	Si le uart est utilisé pour communiquer autre chose que des strings, par exemple
	des données binaires, il se peut très bien que la valeur 0 soit présente dans le buffer.
	Dans ce cas il est impossible de faire la distinction entre une valeur 0 et un \0.

	Si le buffer contient plus de bytes que buffer_length - 1, la fonction va copier
	buffer - 1 bytes et utiliser le dernier byte pour le \0. Par conséquent, le buffer de
	réception ne sera totalement vidé. Pour savoir si cette situation s'est produite, un
	appel à la fonction uart_is_rx_buffer_empty() retournera FALSE.

	Pour garantir que cette situation ne se produira jamais, il suffit de passer un
	buffer plus gros que UART_RX_BUFFER_SIZE
*/
void uart_get_string(uart_e port, char* out_buffer, uint8_t buffer_length);

/**
    \brief Retourne une ligne du buffer: tous les caractères du buffer de réception jusqu'au premier séparateur FIFO_LINE_SEPERATOR
	\param port Le numéro du port du microcontrôleur (UART_0 ou UART_1)
	\return le nb de car de la ligne lue (excluant le \0 final)
	
    Pour que cette fonction puisse fonctionner le buffer qui lui est passé doit minimalement
	avoir deux bytes de long. Un byte pour retourner au moins un caractère et un autre pour
	le \0. Si le buffer est plus petit que ça, cette fonction va corrompre la mémoire sans
	prévenir.

	La fonction termine la string avec un \0 et	retourne.
	S'il n'y a pas de ligne complète dans le buffer, la fonction renvoie un string vide (\0). 
	Comme le UART est relativement très lent par rapport au processeur, il faut
	attendre un peu avant d'appeler cette fonction.

	Si le buffer est vide au moment d'appeler cette fonction, celle-ci retourne une string
	vide terminée par \0

	Si le uart est utilisé pour communiquer autre chose que des strings, par exemple
	des données binaires, il se peut très bien que la valeur 0 soit présente dans le buffer.
	Dans ce cas il est impossible de faire la distinction entre une valeur 0 et un \0.

	Si le buffer contient plus de bytes que buffer_length - 1, la fonction va copier
	buffer - 1 bytes et utiliser le dernier byte pour le \0. Par conséquent, le buffer de
	réception ne sera totalement vidé. Pour savoir si cette situation s'est produite, un
	appel à la fonction uart_is_rx_buffer_empty() retournera FALSE.

	Pour garantir que cette situation ne se produira jamais, il suffit de passer un
	buffer plus gros que UART_RX_BUFFER_SIZE
*/
int uart_get_line(uart_e port, char* out_buffer, uint8_t buffer_length);

/**
    \brief Vide le buffer de réception
	\param port Le numéro du port du microcontrôleur (UART_0 ou UART_1)

	Dans cette situation les bytes reçu sont juste effacés et perdus à jamais.
*/
void uart_clean_rx_buffer(uart_e port);


/**
    \brief Attend que tous les caractères dans le buffer soient envoyés. Cette
	fonction bloque l'éxcécution du code.
	\param port Le numéro du port du microcontrôleur (UART_0 ou UART_1)
*/
void uart_flush(uart_e port);


/**
    \brief Indique si le buffer de réception est vide.
	\param port Le numéro du port du microcontrôleur (UART_0 ou UART_1)
    \return TRUE si il est vide, FALSE s'il contient 1 byte ou plus
*/
bool uart_is_rx_buffer_empty(uart_e port);

/**
    \brief Indique si le buffer de transmission est vide.
	\param port Le numéro du port du microcontrôleur (UART_0 ou UART_1)
    \return TRUE si il est vide, FALSE s'il contient 1 byte ou plus
*/
bool uart_is_tx_buffer_empty(uart_e port);

/**
    \brief Indique le nombre de ligne dans le buffer de réception.
	\param port Le numéro du port du microcontrôleur (UART_0 ou UART_1)
    \return le nombre de ligne dans le buffer
*/
int uart_rx_buffer_nb_line(uart_e port);

#endif // UART_H_INCLUDED
