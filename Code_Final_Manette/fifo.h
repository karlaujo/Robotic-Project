#ifndef FIFO_H_INCLUDED
#define FIFO_H_INCLUDED

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
	@file fifo.h
	@brief Module qui implémente un fifo à longueur statiquement variable
	@author Iouri Savard Colbert
	@date 16 septembre 2012 - Création du module
	@date 24 juillet 2019 - Ajout de fifo_clean

*/

/******************************************************************************
Includes
******************************************************************************/

#include "utils.h"
#include <string.h>

/******************************************************************************
Defines et typedef
******************************************************************************/

typedef struct{

    uint8_t*    ptr;
    uint8_t     size;
    uint8_t     in_offset;
    uint8_t     out_offset;
    bool        is_empty;
    bool        is_full;
	uint8_t		nb_line;	//nombre de ligne dans le buffer (déterminé par nb de FIFO_LINE_SEPERATOR reçus)

} fifo_t;

#define FIFO_LINE_SEPERATOR	'\n'

/******************************************************************************
Prototypes
******************************************************************************/

/**
    \brief Fait l'initialisation de la structure pour le FIFO
	\param fifo Un pointeur sur une structure FIFO vide
	\param ptr_buffer Un pointeur sur une tableau de bytes
	\param buffer_size La grosseur du tableau de bytes
    \return TRUE si il est vide, FALSE s'il contient 1 byte ou plus
*/
void fifo_init(fifo_t* fifo, uint8_t* ptr_buffer, uint8_t buffer_size);
void fifo_push(fifo_t* fifo, uint8_t value);
uint8_t fifo_pop(fifo_t* fifo);
void fifo_clean(fifo_t* fifo);
bool fifo_is_empty(fifo_t* fifo);
bool fifo_is_full(fifo_t* fifo);
int fifo_nb_line(fifo_t* fifo);


#endif // FIFO_H_INCLUDED
