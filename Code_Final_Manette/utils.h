#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

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
	\file
	\brief Header de fonctions utiles à tout projet
	\author Iouri Savard Colbert
	\date 13 Mars 2012 - Création du fichier
	\date 25 juillet 2019 - Ajout de char_array_to_int16() & string_to_int16()										   
	\attention Le fait d'inclure ce header dans un projet inclu automatiquement
	<stdint.h> ce qui rend sont inclusion redondante
*/

/* ----------------------------------------------------------------------------
Includes
---------------------------------------------------------------------------- */

#include <stdint.h>


/* ----------------------------------------------------------------------------
Typedefs
---------------------------------------------------------------------------- */

/**
    \brief défini un type booléen

    En C le type boolen n'existe pas. Il n'a seulement été ajouté en C++. Bien
    que les compilateurs sont la plupart du temps gentils, la vraie manière
    stable et officielle de faire les choses est la suivante
*/
typedef enum {
    FALSE = 0,
    TRUE = !(0)
}bool;


/**
    \brief La seule définition qui nous manque vraiment de stdlib.h c'est celle-ci
*/
#ifndef NULL
    #define NULL 0
#endif


/* ----------------------------------------------------------------------------
Macros
---------------------------------------------------------------------------- */

/* bit operation ----------------------------------------------------------- */

/**
    \brief Pour mettre à 1 un seul bit à l'intérieur d'un byte
	\param[in]  byte    le byte à modifier
    \param[in]  bit     le numréo du bit à modifier à partir de LSB = 0
    \return le byte modifié

    \code
    uint8_t mon_byte= 0b00001111;
    mon_byte = set_bit(mon_byte, 6);
    \endcode

    Produit le résultat suivant :

        mon_byte = 0b01001111
*/
#define set_bit(byte, bit)              ((byte) | (1 << (bit)))


/**
    \brief Pour mettre à 0 un seul bit à l'intérieur d'un byte
	\param[in]  byte    le byte à modifier
    \param[in]  bit     le numréo du bit à modifier à partir de LSB = 0
    \return le byte modifié

    \code
    uint8_t mon_byte= 0b00001111;
    mon_byte = clear_bit(mon_byte, 1);
    \endcode

    Produit le résultat suivant :

        mon_byte = 0b00001101
*/
 #define clear_bit(byte, bit)            ((byte) & ~(1 << (bit)))


/**
    \brief Pour togller un seul bit à l'intérieur d'un byte
	\param[in]  byte    le byte à modifier
    \param[in]  bit     le numréo du bit à modifier à partir de LSB = 0
    \return le byte modifié

    \code
    uint8_t mon_byte= 0b00001111;
    mon_byte = toggle_bit(mon_byte, 1);
    \endcode

    Produit le résultat suivant :

        mon_byte = 0b00001101
*/
#define toggle_bit(byte, bit)           ((byte) ^ (1 << (bit)))


/**
    \brief Pour mettre à une certaine valeur un seul bit à l'intérieur d'un byte
	\param[in]  byte    le byte à modifier
    \param[in]  bit     le numréo du bit à modifier à partir de LSB = 0
    \param[in]  value   la valeur à appliquer (ne peut être que 0 ou 1)
    \return le byte modifié

    \code
    uint8_t mon_byte= 0b00001111;
    mon_byte = write_bit(mon_byte, 1, 0);
    \endcode

    Produit le résultat suivant :

        mon_byte = 0b00001101
*/
#define write_bit(byte, bit, value)     (((byte) & ~(1 << (bit))) | ((value) << (bit)))


/**
    \brief Pour lire un seul bit à l'intérieur d'un byte
	\param[in]  byte    le byte à lire
    \param[in]  bit     le numréo du bit à lire à partir de LSB = 0
    \return le bit lu (ne peut être que 0 ou 1 )

    \code
    uint8_t mon_byte= 0b00001111;
    unit8_t val_retour;
    val_retour = read_bit(mon_byte, 1);
    \endcode

    Produit le résultat suivant :

        val_retour = 1;
*/
#define read_bit(byte, bit)             (((byte) >> (bit)) & 1)

/**
    \brief Pour mettre à 1 plusieurs bit à l'intérieur d'un byte
	\param[in]  byte    le byte à modifier
    \param[in]  mask    le masque des bits qui seront mis à 1
    \return le byte modifié

    \code
    uint8_t mon_byte= 0b00001111;
    mon_byte = set_bits(mon_byte, 0b00111100);
    \endcode

    Produit le résultat suivant :

        mon_byte = 0b00111111;
*/
#define set_bits(byte, mask)            ((byte) | (mask))


/**
    \brief Pour mettre à 0 plusieurs bit à l'intérieur d'un byte
	\param[in]  byte    le byte à modifier
    \param[in]  mask    le masque des bits qui seront mis à 0
    \return le byte modifié

    \code
    uint8_t mon_byte= 0b00001111;
    mon_byte = clear_bits(mon_byte, 0b00111100);
    \endcode

    Produit le résultat suivant :

        mon_byte = 0b00000011;
*/
#define clear_bits(byte, mask)          ((byte) & ~(mask))


/**
    \brief Pour toggler plusieurs bit à l'intérieur d'un byte
    \param[in]  byte    le byte à modifier
    \param[in]  mask    le masque des bits qui seront basculés
    \return le byte modifié

    \code
    uint8_t mon_byte= 0b00001111;
    mon_byte = toggle_bits(mon_byte, 0b00111100);
    \endcode

    Produit le résultat suivant :

        mon_byte = 0b00110011;
*/
#define toggle_bits(byte, mask)         ((byte) ^ (mask))


/**
    \brief Pour mettre à une certaine valeur plusieurs bit à l'intérieur d'un byte
    \param[in]  byte    le byte à modifier
    \param[in]  mask    le masque des bits qui seront modifiées
    \param[in]  value   la valeur à appliquer
    \return le byte modifié

    \code
    uint8_t mon_byte= 0b00001111;
    mon_byte = write_bits(mon_byte, 0b00111100, 0b01010101);
    \endcode

    Produit le résultat suivant :

        mon_byte = 0b00010111;
*/
#define write_bits(byte, mask, value)   (((byte) & ~(mask)) | ((value) & (mask)))


/**
    \brief Pour lire la valeur de plusieurs bit à l'intérieur d'un byte avec les bits
    non intéressants masqués à 0
	\param[in]  byte    le byte à lire
    \param[in]  mask    le masque des bits qui seront réèlement lus
    \return la valeur masquée

    \code
    uint8_t mon_byte= 0b00001111;
    unit8_t val_retour;
    val_retour = read_bits(mon_byte, 0b00111100);
    \endcode

    Produit le résultat suivant :

        val_retour = 0b00001100;
*/
#define read_bits(byte, mask)           ((byte) & (mask))


/* Math *********************************************************************/

/**
    \brief Retourne la valeur absolue d'un nombre
	\param[in]  value    Le nombre à rendre positif
    \return le nombre en valeur absolue
*/
#define abs(value)                          ((value) < 0? -(value) : (value))



/* ----------------------------------------------------------------------------
Prototypes
---------------------------------------------------------------------------- */

/* Memory management **********************************************************/

/**
    \brief Copie un bloc de mémoire à un autre endroit
    \param[out] destination Pointeur sur la destination de la copie
    \param[in]  source  Pointeur sur la source de la copie
    \param[in]  num     le nombre de byte à copier
    \attention num c'est le nombre de bytes et non pas le nombte d'élément d'un
    tableau.  Pour copier un tableau qui conteint autre chose que des bytes,
    l'argument num devrait avoir la forme suivante :

        nb_elements * sizeof(element)
*/
void mem_copy(void * destination, const void * source, uint8_t num );


/* String stuff *************************************************************/

/**
    \brief Retourne la longueur d'une string (array terminé par '\0')
    \param[in]  string  Pointeur sur la string
    \return la longueur de la string sans le caractère nul

    \code
    uint8_t ret_val;
    ret_val = string_length("Hello World");
    \endcode

    Produira la résultat suivant :

    > ret_val = 11
*/
uint8_t string_length(const char* string);

/**
    \brief Retourne la longueur d'une string (array terminé par '\0')
    \param[out] out_string  Pointeur sur la string de destination
    \param[in]  in_string   Pointeur sur la string de source
    \return Le nombre de caractères qui ont été copiés, sans compter le '\0'
*/
uint8_t string_copy(char* out_string, const char* in_string);


/* Conversion text to number ************************************************/

/**
    \brief Converti un caractère représentant un chiffre en sa valeur
    \param[in]  character   Le caractère en question
    \return La valeur ou zéro si le caractère n'avait pas rapport
*/
uint8_t char_to_uint(char character);


/**
    \brief Converti une string représentant un nombre en sa valeur
    \param[in]  string  La string en question
    \return La valeur ou de la cochonnerie si la string n'avait pas rapport
*/
uint32_t string_to_uint(const char* string);


/**
    \brief Converti un array de char représentant un nombre en sa valeur
    \param[in]  char_array  Le array en question
    \param[in]  size    La grosseur intéressante du array puisque celui n'est pas
    obligé de se terminer par la caractère nul
    \return La valeur ou de la cochonnerie si le array n'avait pas rapport
*/
uint32_t char_array_to_uint(const char* char_array, uint8_t size);


/**
    \brief Converti un caractère représentant un chiffre hexadécimal en sa valeur
    \param[in]  character   Le caractère en question
    \return La valeur ou zéro si le caractère n'avait pas rapport
*/
uint8_t hex_char_to_uint(char character);


/**
    \brief Converti une string représentant un nombre hexadécimal en sa valeur
    \param[in]  string  La string en question
    \return La valeur ou de la cochonnerie si la string n'avait pas rapport
*/
uint32_t hex_string_to_uint(const char* string);


/**
    \brief Converti un array de char représentant un nombre hexadécimal en sa valeur
    \param[in]  char_array  Le array en question
    \param[in]  size    La grosseur intéressante du array puisque celui n'est pas
    obligé de se terminer par la caractère nul
    \return La valeur ou de la cochonnerie si le array n'avait pas rapport
*/
uint32_t hex_char_array_to_uint(const char* char_array, uint8_t size);


/**
    \brief Converti un array de char représentant un nombre signé en sa valeur
    \param[in]  char_array  Le array en question
    \param[in]  size    La grosseur intéressante du array puisque celui n'est pas
    obligé de se terminer par la caractère nul
    \return La valeur ou de la cochonnerie si le array n'avait pas rapport
*/
int16_t char_array_to_int16(const char* char_array, uint8_t size);

/**
    \brief Converti une string représentant un nombre signé en sa valeur
    \param[in]  string  La string en question
    \return La valeur ou de la cochonnerie si la string n'avait pas rapport
*/
int16_t string_to_int16(const char* string);


/* Conversion number to text ************************************************/

/**
    \brief Converti un nombre en le caractère ASCII qui le représente
    \param[in]  digit   Le nombre entre 0 et 9
    \return Le caractère ASCII qui repésente le paramètre
    \warning C'est une erreur d'appeler cette fonction avec un paramètre qui est négatif
    ou plus grand que 9
*/
char uint_to_char(uint8_t digit);

/**
    \brief Converti un nombre hexdécimal en le caractère ASCII qui le représente
    \param[in]  hex_digit   Le nombre entre 0 et 15
    \return Le caractère ASCII qui repésente le paramètre
    \warning C'est une erreur d'appeler cette fonction avec un paramètre qui est négatif
    ou plus grand que 15

    Pour les nombre entre 10 et 15, la sortie est une lettre majuscule entre A et F
*/
char uint_to_hex_char(uint8_t hex_digit);

/**
    \brief Converti un entier non signé de 8 bits en une string
    \param[out] out_string  La string de destination
    \param[in]  number      Le nombre à convertir
    \return     Le nombre de caractères ajoutés à la string sans compter le '\0'
    \warning    La string doit être assez longue pour contenir la conversion. Dans le
    cas d'un 8 bits, ça prend une string qui fait minimalement 4 caractères de long.

    La string de sortie a toujours 3 caractères et ce peu importe la longueur du nombre.
    Par conséquent, si le nombre est plus petit que 100, le résultat sera "paddé" de 0

    Le retour de la fonction permet de faire des appels successifs et de progressivement
    construire une string complexe

    \code

    char string[16];
    uint8_t string_index;
    string_index = uint8_to_string(string, 42);

    string[string_index] = '!';
    string[string_index + 1] = '\0';

    \endcode

    produira la string suivante :

        042!

*/
uint8_t uint8_to_string(char* out_string, uint8_t number);

/**
    \brief Converti un entier non signé de 16 bits en une string
    \param[out] out_string  La string de destination
    \param[in]  number      Le nombre à convertir
    \return     Le nombre de caractères ajoutés à la string sans compter le '\0'
    \warning    La string doit être assez longue pour contenir la conversion. Dans le
    cas d'un 16 bits, ça prend une string qui fait minimalement 6 caractères de long.

    La string de sortie a toujours 5 caractères et ce peu importe la longueur du nombre.
    Par conséquent, si le nombre est plus petit que 10 000, le résultat sera "paddé" de 0

    Le retour de la fonction permet de faire des appels successifs et de progressivement
    construire une string complexe

    \code

    char string[16];
    uint8_t string_index;
    string_index = uint16_to_string(string, 42);

    string[string_index] = '!';
    string[string_index + 1] = '\0';

    \endcode

    produira la string suivante :

        00042!

*/
uint8_t uint16_to_string(char* out_string, uint16_t number);

/**
    \brief Converti un entier non signé de 32 bits en une string
    \param[out] out_string  La string de destination
    \param[in]  number      Le nombre à convertir
    \return     Le nombre de caractères ajoutés à la string sans compter le '\0'
    \warning    La string doit être assez longue pour contenir la conversion. Dans le
    cas d'un 32 bits, ça prend une string qui fait minimalement 11 caractères de long.

    La string de sortie a toujours 10 caractères et ce peu importe la longueur du nombre.
    Par conséquent, si le nombre est plus petit que 1 000 000 000, le résultat sera "paddé" de 0

    Le retour de la fonction permet de faire des appels successifs et de progressivement
    construire une string complexe

    \code

    char string[16];
    uint8_t string_index;
    string_index = uint32_to_string(string, 42);

    string[string_index] = '!';
    string[string_index + 1] = '\0';

    \endcode

    produira la string suivante :

        0000000042!

*/
uint8_t uint32_to_string(char* out_string, uint32_t number);

/**
    \brief Converti un entier hexadécimal de 8 bits en une string
    \param[out] out_string  La string de destination
    \param[in]  number      Le nombre à convertir
    \return     Le nombre de caractères ajoutés à la string sans compter le '\0'
    \warning    La string doit être assez longue pour contenir la conversion. Dans le
    cas d'un 8 bits, ça prend une string qui fait minimalement 3 caractères de long.

    Le retour de la fonction permet de faire des appels successifs et de progressivement
    construire une string complexe

    \code

    char string[16];
    uint8_t string_index;
    string_index = uint8_to_hex_string(string, 0x2A);

    string[string_index] = '!';
    string[string_index + 1] = '\0';

    \endcode

    produira la string suivante :

        2A!

*/
uint8_t uint8_to_hex_string(char* out_string, uint8_t number);

/**
    \brief Converti un entier hexadécimal de 16 bits en une string
    \param[out] out_string  La string de destination
    \param[in]  number      Le nombre à convertir
    \return     Le nombre de caractères ajoutés à la string sans compter le '\0'
    \warning    La string doit être assez longue pour contenir la conversion. Dans le
    cas d'un 16 bits, ça prend une string qui fait minimalement 5 caractères de long.

    Le retour de la fonction permet de faire des appels successifs et de progressivement
    construire une string complexe

    \code

    char string[16];
    uint8_t string_index;
    string_index = uint16_to_hex_string(string, 0x002A);

    string[string_index] = '!';
    string[string_index + 1] = '\0';

    \endcode

    produira la string suivante :

        002A!

*/
uint8_t uint16_to_hex_string(char* out_string, uint16_t number);

/**
    \brief Converti un entier hexadécimal de 32 bits en une string
    \param[out] out_string  La string de destination
    \param[in]  number      Le nombre à convertir
    \return     Le nombre de caractères ajoutés à la string sans compter le '\0'
    \warning    La string doit être assez longue pour contenir la conversion. Dans le
    cas d'un 16 bits, ça prend une string qui fait minimalement 9 caractères de long.

    Le retour de la fonction permet de faire des appels successifs et de progressivement
    construire une string complexe

    \code

    char string[16];
    uint8_t string_index;
    string_index = uint32_to_hex_string(string, 0x0000002A);

    string[string_index] = '!';
    string[string_index + 1] = '\0';

    \endcode

    produira la string suivante :

        0000002A!

*/
uint8_t uint32_to_hex_string(char* out_string, uint32_t number);

/**
    \brief Converti un entier signé de 8 bits en une string
    \param[out] out_string  La string de destination
    \param[in]  number      Le nombre à convertir
    \return     Le nombre de caractères ajoutés à la string sans compter le '\0'
    \warning    La string doit être assez longue pour contenir la conversion. Dans le
    cas d'un 8 bits, ça prend une string qui fait minimalement 4 caractères de long.

    La string de sortie aura le signe comme préfixe suivit de 3 caractères repésentant
    la valeur absolue du nombre. Le préfixe peu être '+' ou '-'. Si le nombre est plus
    petit que 100 et plus grand que -100, le résultat sera "paddé" de 0

    Le retour de la fonction permet de faire des appels successifs et de progressivement
    construire une string complexe

    \code

    char string[16];
    uint8_t string_index;
    string_index = int8_to_string(string, -42);

    string[string_index] = '!';
    string[string_index + 1] = '\0';

    \endcode

    produira la string suivante :

        -042!

*/
uint8_t int8_to_string(char* out_string, int8_t number);

/**
    \brief Converti un entier signé de 16 bits en une string
    \param[out] out_string  La string de destination
    \param[in]  number      Le nombre à convertir
    \return     Le nombre de caractères ajoutés à la string sans compter le '\0'
    \warning    La string doit être assez longue pour contenir la conversion. Dans le
    cas d'un 16 bits, ça prend une string qui fait minimalement 7 caractères de long.

    La string de sortie aura le signe comme préfixe suivit de 3 caractères repésentant
    la valeur absolue du nombre. Le préfixe peu être '+' ou '-'. Si le nombre est plus
    petit que 10000 et plus grand que -10000, le résultat sera "paddé" de 0

    Le retour de la fonction permet de faire des appels successifs et de progressivement
    construire une string complexe

    \code

    char string[16];
    uint8_t string_index;
    string_index = int16_to_string(string, -42);

    string[string_index] = '!';
    string[string_index + 1] = '\0';

    \endcode

    produira la string suivante :

        -00042!

*/
uint8_t int16_to_string(char* out_string, int16_t number);

/**
    \brief Converti un entier signé de 32 bits en une string
    \param[out] out_string  La string de destination
    \param[in]  number      Le nombre à convertir
    \return     Le nombre de caractères ajoutés à la string sans compter le '\0'
    \warning    La string doit être assez longue pour contenir la conversion. Dans le
    cas d'un 32 bits, ça prend une string qui fait minimalement 12 caractères de long.

    La string de sortie aura le signe comme préfixe suivit de 3 caractères repésentant
    la valeur absolue du nombre. Le préfixe peu être '+' ou '-'. Si le nombre est plus
    petit que 1 000 000 000 et plus grand que -1 000 000 000, le résultat sera "paddé" de 0

    Le retour de la fonction permet de faire des appels successifs et de progressivement
    construire une string complexe

    \code

    char string[16];
    uint8_t string_index;
    string_index = int32_to_string(string, -42);

    string[string_index] = '!';
    string[string_index + 1] = '\0';

    \endcode

    produira la string suivante :

        -0000000042!

*/
uint8_t int32_to_string(char* out_string, int32_t number);


#endif // UTILS_H_INCLUDED
