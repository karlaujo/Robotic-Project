#ifndef DRIVER_H_INCLUDED
#define DRIVER_H_INCLUDED

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
	\brief Header de fonctions qui pilotent directement du matériel
	\author Iouri Savard Colbert
	\date 24 janvier 2018
*/

/* ----------------------------------------------------------------------------
Includes
---------------------------------------------------------------------------- */

#include "utils.h"

/* ----------------------------------------------------------------------------
Prototypes
---------------------------------------------------------------------------- */

/**
    \brief Initialise le module de l'ADC
    \return rien.

	Le module de l'ADC utilise le PORT A. 

			    +---- ----+
			  --| 1  U 40 |-- ADC 0
			  --| 2    39 |-- ADC 1
			  --| 3    38 |-- ADC 2
			  --| 4    37 |-- ADC 3
			  --| 5    36 |-- ADC 4
			  --| 6    35 |-- ADC 5
			  --| 7    34 |-- ADC 6
			  --| 8    33 |-- ADC 7
			  --| 9    32 |--
			  --| 10   31 |--
			  --| 11   30 |--
			  --| 12   29 |--
			  --| 13   28 |--
			  --| 14   27 |--
			  --| 15   26 |--
			  --| 16   25 |--
			  --| 17   24 |--
	          --| 18   23 |--
			  --| 19   22 |--
			  --| 20   21 |--
			    +---------+
*/
void adc_init(void);


/**
    \brief Fait une conversion de la valeur analogique présente sur une entrée
    \param[in]	channel	Le channel sur lequel la conversion doit être effectuée (entre PA0 et PA7 inclusivement)
    \return La valeur convertie.

	C'est une erreur d'appeler cette fonction avec un channel négatif ou plus grand que 7

	Il est important de noter que cette fonction ne s'exécute pas instantanément. La conversion
	prend un certain temps à s'effectuer et la fonction attend la fin de la conversion avant de
	retourner. C'est une mauvaise idée d'appeler cette fonction dans une boucle avec des temps
	critiques.
*/
uint8_t adc_read(uint8_t channel);

/**
    \brief  Fait l'initialisation des registres nécéssaires à la génération de modulation de largeur d'impulsion (PWM)
    \return rien.

	Cette fonction détermine le fonctionnement du compteur 0 (8 bits) de l'ATmega324A pour générer des signaux MLI 
	sur les broches PB3 et PB4.
*/
void pwm0_init(void);

/**
    \brief Détermine le rapport cyclique du PWM à la sortie PB3 (broche 4 du DIP)
	\param[in]	duty Une valeur entre 0 et 255
    \return rien.

	Un duty (rapport cyclique) de 0 correspond à un PWM de 0% et un duty de 255 correspond à un
	PWM de 100%. Cette relation est linéaire sur tout l'intervalle.

			    +---- ----+
			  --| 1  U 40 |--
			  --| 2    39 |--
			  --| 3    38 |--
		  PB3 --| 4    37 |--
			  --| 5    36 |--
			  --| 6    35 |--
			  --| 7    34 |--
			  --| 8    33 |--
			  --| 9    32 |--
			  --| 10   31 |--
			  --| 11   30 |--
			  --| 12   29 |--
			  --| 13   28 |--
			  --| 14   27 |--
			  --| 15   26 |--
			  --| 16   25 |--
			  --| 17   24 |--
	          --| 18   23 |--
			  --| 19   22 |--
			  --| 20   21 |--
			    +---------+
*/
void pwm0_set_PB3(uint8_t duty);

/**
    \brief Détermine le rapport cyclique du PWM à la sortie PB4 (broche 5 du DIP)
	\param[in]	duty Une valeur entre 0 et 255
    \return rien.

	Un duty (rapport cyclique) de 0 correspond à un PWM de 0% et un duty de 255 correspond à un
	PWM de 100%. Cette relation est linéaire sur tout l'intervalle.

			    +---- ----+
			  --| 1  U 40 |--
			  --| 2    39 |--
			  --| 3    38 |--
		      --| 4    37 |--
		 PB4  --| 5    36 |--
			  --| 6    35 |--
			  --| 7    34 |--
			  --| 8    33 |--
			  --| 9    32 |--
			  --| 10   31 |--
			  --| 11   30 |--
			  --| 12   29 |--
			  --| 13   28 |--
			  --| 14   27 |--
			  --| 15   26 |--
			  --| 16   25 |--
			  --| 17   24 |--
	          --| 18   23 |--
			  --| 19   22 |--
			  --| 20   21 |--
			    +---------+
*/
void pwm0_set_PB4(uint8_t duty);

/**
    \brief  Fait l'initialisation des registres nécéssaires à la génération de modulation de largeur d'impulsion (PWM)
	\param[in]	top Une valeur entre 0 et 65535 qui détermine la valeur maximale du compteur
    \return rien.

	Cette fonction détermine le fonctionnement du compteur 1 (16 bits) de l'ATmega324A pour générer des signaux MLI 
	sur les broches PD4 et PD5. En jouant sur la valeur top, on peut ajuster la période du signal MLI.
*/
void pwm1_init(uint16_t top);

/**
    \brief Détermine le rapport cyclique du PWM à la sortie PD4 (broche 21 du DIP)
	\param[in]	duty Une valeur entre 0 et 65535
    \return rien.

	Le rapport cyclique dépend de la valeur top fixée à l'initialisation et de la valeur duty de cette fonction. 
	RC=duty/top
	Exe.: si top vaut 100, et duty vaut 50, le rapport cyclique est de 50%.

			    +---- ----+
			  --| 1  U 40 |--
			  --| 2    39 |--
			  --| 3    38 |--
			  --| 4    37 |--
			  --| 5    36 |--
			  --| 6    35 |--
			  --| 7    34 |--
			  --| 8    33 |--
			  --| 9    32 |--
			  --| 10   31 |--
			  --| 11   30 |--
			  --| 12   29 |--
			  --| 13   28 |--
			  --| 14   27 |--
			  --| 15   26 |--
			  --| 16   25 |--
			  --| 17   24 |--
	      PD4 --| 18   23 |--
			  --| 19   22 |--
			  --| 20   21 |-- 
			    +---------+
*/
void pwm1_set_PD4(uint16_t duty);

/**
    \brief Détermine le rapport cyclique du PWM à la sortie PD5 (broche 21 du DIP)
	\param[in]	duty Une valeur entre 0 et 65535
    \return rien.

	Le rapport cyclique dépend de la valeur top fixée à l'initialisation et de la valeur duty de cette fonction. 
	RC=duty/top
	Exe.: si top vaut 100, et duty vaut 50, le rapport cyclique est de 50%.

			    +---- ----+
			  --| 1  U 40 |--
			  --| 2    39 |--
			  --| 3    38 |--
			  --| 4    37 |--
			  --| 5    36 |--
			  --| 6    35 |--
			  --| 7    34 |--
			  --| 8    33 |--
			  --| 9    32 |--
			  --| 10   31 |--
			  --| 11   30 |--
			  --| 12   29 |--
			  --| 13   28 |--
			  --| 14   27 |--
			  --| 15   26 |--
			  --| 16   25 |--
			  --| 17   24 |--
	          --| 18   23 |--
		  PD5 --| 19   22 |--
			  --| 20   21 |--
			    +---------+
*/
void pwm1_set_PD5(uint16_t duty);

/**
    \brief  Fait l'initialisation des registres nécéssaires à la génération de modulation de largeur d'impulsion (PWM)
    \return rien.

	Cette fonction détermine le fonctionnement du compteur 2 (8 bits) l'ATmega324A pour générer des signaux MLI sur les broches
	PD6 et PD7.
*/
void pwm2_init(void);

/**
    \brief Détermine le rapport cyclique du PWM à la sortie PD7 (broche 21 du DIP)
	\param[in]	duty Une valeur entre 0 et 255
    \return rien.

	Un duty (rapport cyclique) de 0 correspond à un PWM de 0% et un duty de 255 correspond à un
	PWM de 100%. Cette relation est linéaire sur toute l'intervalle.

			    +---- ----+
			  --| 1  U 40 |--
			  --| 2    39 |--
			  --| 3    38 |--
			  --| 4    37 |--
			  --| 5    36 |--
			  --| 6    35 |--
			  --| 7    34 |--
			  --| 8    33 |--
			  --| 9    32 |--
			  --| 10   31 |--
			  --| 11   30 |--
			  --| 12   29 |--
			  --| 13   28 |--
			  --| 14   27 |--
			  --| 15   26 |--
			  --| 16   25 |--
			  --| 17   24 |--
	          --| 18   23 |--
			  --| 19   22 |--
			  --| 20   21 |-- PD7
			    +---------+
*/
void pwm2_set_PD7(uint8_t duty);

/**
    \brief Détermine le rapport cyclique du PWM à la sortie PD6 (broche 20 du DIP)
	\param[in]	duty Une valeur entre 0 et 255
    \return rien.

	Un duty (rapport cyclique) de 0 correspond à un PWM de 0% et un duty de 255 correspond à un
	PWM de 100%. Cette relation est linéaire sur toute l'intervalle.

			    +---- ----+
			  --| 1  U 40 |--
			  --| 2    39 |--
			  --| 3    38 |--
			  --| 4    37 |--
			  --| 5    36 |--
			  --| 6    35 |--
			  --| 7    34 |--
			  --| 8    33 |--
			  --| 9    32 |--
			  --| 10   31 |--
			  --| 11   30 |--
			  --| 12   29 |--
			  --| 13   28 |--
			  --| 14   27 |--
			  --| 15   26 |--
			  --| 16   25 |--
			  --| 17   24 |--
	          --| 18   23 |--
			  --| 19   22 |--
		  PD6 --| 20   21 |--
			    +---------+
*/
void pwm2_set_PD6(uint8_t duty);

/**
    \brief Initialise le contrôle des moteurs
    \return rien.
*/

#endif /* DRIVER_H_INCLUDED */
