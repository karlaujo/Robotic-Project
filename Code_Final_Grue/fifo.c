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
	@file fifo.c
	@brief Module qui implémente un fifo à longueur statiquement variable
	@author Iouri Savard Colbert
	@date 16 septembre 2012 - Création du module
	@date 24 juillet 2019 - Ajout de fifo_clean
*/

/******************************************************************************
Includes
******************************************************************************/

#include "fifo.h"
#include "lcd.h"
#include <stdio.h>
#include <avr/io.h>
#include <string.h>


/******************************************************************************
Global functions
******************************************************************************/

void fifo_init(fifo_t* fifo, uint8_t* ptr_buffer, uint8_t buffer_size){

    fifo->ptr = ptr_buffer;
    fifo->size = buffer_size;
    fifo->in_offset = 0;
    fifo->out_offset = 0;
    fifo->is_empty = TRUE;
    fifo->is_full = FALSE;
	fifo->nb_line = 0;
}


void fifo_push(fifo_t* fifo, uint8_t value){
	char str[20];
	//lcd_write_char(value);
    /* Si le buffer est plein il n'est pas question de rien "pusher" */
    if(fifo->is_full == FALSE){

        fifo->ptr[fifo->in_offset] = value;

        fifo->is_empty = FALSE;

        if(fifo->in_offset == fifo->size - 1){

            fifo->in_offset = 0;
        }

        else{

            fifo->in_offset++;
        }
		//lcd_write_char(value);
		/* gestion du nombre de lignes */
		if(value==FIFO_LINE_SEPERATOR){
			 fifo->nb_line++;
			 /*lcd_set_cursor_position(0,1);
			 sprintf(str,"%d",fifo->nb_line);
			 lcd_write_string(str);
			 PORTB=set_bit(PORTB,PB1);*/
		}
		
        /* si l'index de d'entrée ratrappe celui de sortie c'est que le buffer est plein */
        if(fifo->in_offset == fifo->out_offset){

            fifo->is_full = TRUE;
        }
    }
}


uint8_t fifo_pop(fifo_t* fifo){
	//char str[40];
    uint8_t value;

    /* Si le buffer n'est pas vide il n'est pas question de rien "poper" */
    if(fifo->is_empty == FALSE){

        value = fifo->ptr[fifo->out_offset];

        fifo->is_full = FALSE;

        if(fifo->out_offset == fifo->size - 1){

            fifo->out_offset = 0;
        }

        else{

            fifo->out_offset++;
        }
		
		/* gestion du nombre de lignes */
		if(value==FIFO_LINE_SEPERATOR){
			 fifo->nb_line--;
			 /*lcd_set_cursor_position(0,1);
			 sprintf(str,"%d",fifo->nb_line);
			 lcd_write_string(str);*/
			 //PORTB=set_bit(PORTB,PB2);
		}
		
        /* si l'index de sortie ratrappe celui d'entrée c'est que le buffer est vide */
        if(fifo->out_offset == fifo->in_offset){

            fifo->is_empty = TRUE;
        }
    }

    else{

        /* En orienté objet je ferais une exception, mais en c le mieux que je peux faire
        c'est ça */
        value = 0;
    }

    return value;
}


void fifo_clean(fifo_t* fifo){
	
	fifo->in_offset = fifo->out_offset;
	fifo->is_full = FALSE;
	fifo->is_empty = TRUE;
}


bool fifo_is_empty(fifo_t* fifo) {

    return fifo->is_empty;
}


bool fifo_is_full(fifo_t* fifo){

    return fifo->is_full;
}

int fifo_nb_line(fifo_t* fifo){

	return fifo->nb_line;
}