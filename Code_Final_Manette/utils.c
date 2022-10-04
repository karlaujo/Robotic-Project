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

	\file utils.c
	\brief Header de fonctions utiles à tout projet
	\author Iouri Savard Colbert
	\date 13 Mars 2012
*/

/******************************************************************************
Includes
******************************************************************************/

#define DISABLE_UTILS_H_MACRO /* Obligatoire ici */
#include "utils.h"


/** Memory management **********************************************************/
void mem_copy(void * destination, const void * source, uint8_t num ){

    uint8_t i;

    for(i = 0; i < num; i++){

        ((uint8_t*)destination)[i] = ((uint8_t*)source)[i];
    }
}


/** String stuff *************************************************************/
uint8_t string_length(const char* string){

    uint8_t length = 0;

    while(string[length] != '\0'){

        length++;
    }

    return length;
}

uint8_t string_copy(char* out_string, const char* in_string){
	
	uint8_t i = 0;
	
	while(in_string[i] != '\0'){
		
		out_string[i] = in_string[i];
		i++;
	}
	
	// Puisque le while juste avant abandonne au \0, il faut le mettre
	out_string[i] = '\0';
	
	return i;
}


/** Conversion text to number ************************************************/
uint8_t char_to_uint(char caracter){

    uint8_t digit = 0;

    if((caracter >= '0') && (caracter <= '9')){

        digit = caracter - '0';
    }

    return digit;
}


uint32_t string_to_uint(const char* string){

    uint8_t string_index;
    uint32_t power_of_ten = 1;
    uint32_t converted_uint = 0;

    string_index = string_length(string);

    while(string_index > 0){

        converted_uint += (char_to_uint(string[string_index - 1])) * power_of_ten;
        string_index--;
        power_of_ten = power_of_ten * 10;
    }

    return converted_uint;
}

uint32_t char_array_to_uint(const char* char_array, uint8_t size){

    uint32_t power_of_ten = 1;
    uint32_t converted_uint = 0;

    while(size > 0){

        converted_uint += (char_to_uint(char_array[size - 1])) * power_of_ten;
        size--;
        power_of_ten = power_of_ten * 10;
    }

    return converted_uint;

}

uint8_t hex_char_to_uint(char caracter){

    uint8_t nibble = 0;

    if((caracter >= '0') && (caracter <= '9')){

        nibble = caracter - '0';
    }

    else if((caracter >= 'a') && (caracter <= 'f')){

        nibble = caracter - 'a' + 0xA;
    }

    return nibble;
}


uint32_t hex_string_to_uint(const char* string){

    uint8_t string_index;
    uint8_t nibble_count = 0;
    uint32_t converted_uint = 0;

    string_index = string_length(string);

    while(string_index > 0){

        converted_uint |= (hex_char_to_uint(string[string_index - 1])) << (4 * nibble_count);
        string_index--;
        nibble_count++;
    }

    return converted_uint;
}

uint32_t hex_char_array_to_uint(const char* char_array, uint8_t size){

    uint8_t nibble_count = 0;
    uint32_t converted_uint = 0;

    while(size > 0){

        converted_uint |= (hex_char_to_uint(char_array[size - 1])) << (4 * nibble_count);
        size--;
        nibble_count++;
    }

    return converted_uint;
}

int16_t char_array_to_int16(const char* char_array, uint8_t size)
{
    bool is_negative = FALSE;
    uint16_t absolute_value;

    // Si le tableau a une longueur raisonable (un signe plus minimum un char)
    if(size >= 2)
    {
        if(char_array[0] == '-')
        {
            is_negative = TRUE;
        }
        else if(char_array[0] == '+')
        {
            is_negative = FALSE;
        }
        else    // Erreur
        {
            return 0;
        }
    }
    else    // Erreur
    {
        return 0;
    }

    absolute_value = char_array_to_uint(&(char_array[1]), size - 1);

    return is_negative? -absolute_value : absolute_value;

}

int16_t string_to_int16(const char* string){

    uint8_t size;

    size = string_length(string);

    return char_array_to_int16(string, size);
}


/** Conversion number to text ************************************************/

char uint_to_char(uint8_t digit){

    char caracter = '\0';

    if((digit >= 0) && (digit <= 9)){

        caracter = digit + '0';
    }

    return caracter;
}


char uint_to_hex_char(uint8_t hex_digit){

	char caracter = '\0';

	if((hex_digit >= 0x00) && (hex_digit <= 0x09)){

		caracter = hex_digit + '0';
	}
	
	else if((hex_digit >= 0x0A) && (hex_digit <= 0x0F)){
		
		caracter = hex_digit + 'A';
	}

	return caracter;
}


uint8_t uint8_to_string(char* out_string, uint8_t number){

    uint8_t anti_rest;  //après avoir fixé mon écran dans le vide pendant 15 minutes à chercher
    // un nom intelligent pour cette variable j'ai décidé de passer à l'action et de lui donner
    // le premier nom qui me passerait par la tête.
    uint8_t string_index = 0;
    uint8_t power_of_ten = 100;


    while(power_of_ten > 0){

        anti_rest = number / power_of_ten;

        out_string[string_index] = uint_to_char(anti_rest);

        number -= anti_rest * power_of_ten;

        string_index++;

        power_of_ten /= 10;
    }

    /* On ferme la string */
    out_string[string_index] = '\0';
	
	return string_index;
}


uint8_t uint16_to_string(char* out_string, uint16_t number){

    uint8_t anti_rest;  //après avoir fixé mon écran dans le vide pendant 15 minutes à chercher
    // un nom intelligetn pour cette variable j'ai décidé de passer à l'action et de lui donner
    // le premier nom qui me passerait par la tête.
    uint8_t string_index = 0;
    uint16_t power_of_ten = 10000;


    while(power_of_ten > 0){

        anti_rest = number / power_of_ten;

        out_string[string_index] = uint_to_char(anti_rest);

        number -= anti_rest * power_of_ten;

        string_index++;

        power_of_ten /= 10;
    }

    /* On ferme la string */
    out_string[string_index] = '\0';
	
	return string_index;
}


uint8_t uint32_to_string(char* out_string, uint32_t number){

    uint8_t anti_rest;  //après avoir fixé mon écran dans le vide pendant 15 minutes à chercher
    // un nom intelligetn pour cette variable j'ai décidé de passer à l'action et de lui donner
    // le premier nom qui me passerait par la tête.
    uint8_t string_index = 0;
    uint32_t power_of_ten = 1000000000UL;


    while(power_of_ten > 0){

        anti_rest = number / power_of_ten;

        out_string[string_index] = uint_to_char(anti_rest);

        number -= anti_rest * power_of_ten;

        string_index++;

        power_of_ten /= 10;
    }

    /* On ferme la string */
    out_string[string_index] = '\0';
	
	return string_index;
}


uint8_t uint8_to_hex_string(char* out_string, uint8_t number){
	 
	out_string[0] = uint_to_hex_char((number >> 4) & 0b00001111);
	out_string[1] = uint_to_hex_char(number & 0b00001111);
	out_string[2] = '\0';
	
	return 2;
}


uint8_t uint16_to_hex_string(char* out_string, uint16_t number){
	
	out_string[0] = uint_to_hex_char((uint8_t)((number >> 12) & 0b0000000000001111));
	out_string[1] = uint_to_hex_char((uint8_t)((number >> 8) & 0b0000000000001111));
	out_string[2] = uint_to_hex_char((uint8_t)((number >> 4) & 0b0000000000001111));
	out_string[3] = uint_to_hex_char((uint8_t)(number & 0b0000000000001111));
	out_string[4] = '\0';
		 
	return 4;
}


uint8_t uint32_to_hex_string(char* out_string, uint32_t number){
	
	out_string[0] = uint_to_hex_char((uint8_t)((number >> 28) & 0b00000000000000000000000000001111));
	out_string[1] = uint_to_hex_char((uint8_t)((number >> 24) & 0b00000000000000000000000000001111));
	out_string[2] = uint_to_hex_char((uint8_t)((number >> 20) & 0b00000000000000000000000000001111));
	out_string[3] = uint_to_hex_char((uint8_t)((number >> 16) & 0b00000000000000000000000000001111));
	out_string[4] = uint_to_hex_char((uint8_t)((number >> 12) & 0b00000000000000000000000000001111));
	out_string[5] = uint_to_hex_char((uint8_t)((number >> 8) & 0b00000000000000000000000000001111));
	out_string[6] = uint_to_hex_char((uint8_t)((number >> 4) & 0b00000000000000000000000000001111));
	out_string[7] = uint_to_hex_char((uint8_t)(number & 0b00000000000000000000000000001111));
	out_string[8] = '\0';
	
	return 8;
}


uint8_t int8_to_string(char* out_string, int8_t number){

    /* Détermination du signe */
    if(number < 0){

        out_string[0] = '-';
    }
    else{

        out_string[0] = '+';
    }

    /* Une fois que le signe est sorti, reste juste à convertir la valeur absolue du reste */
	// Le +1 c'est pour tenir compte du signe
    return uint8_to_string(&out_string[1], (uint8_t)abs(number)) + 1;
}


uint8_t int16_to_string(char* out_string, int16_t number){

    /* Détermination du signe */
    if(number < 0){

        out_string[0] = '-';
    }
    else{

        out_string[0] = '+';
    }

    /* Une fois que le signe est sorti, reste juste à convertir la valeur absolue du reste */
	// Le +1 c'est pour tenir compte du signe
    return uint16_to_string(&out_string[1], (uint16_t)abs(number)) + 1;

}


uint8_t int32_to_string(char* out_string, int32_t number){

    /* Détermination du signe */
    if(number < 0){

        out_string[0] = '-';
    }
    else{

        out_string[0] = '+';
    }

    /* Une fois que le signe est sorti, reste juste à convertir la valeur absolue du reste */
	// Le +1 c'est pour tenir compte du signe
    return uint32_to_string(&out_string[1], (uint32_t)abs(number)) + 1;

}
