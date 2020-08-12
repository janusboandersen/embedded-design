/*
 * rgbLed.h
 *
 *  Created on: 9 Sep 2019
 *      Author: janusboandersen
 */

#ifndef RGBLED_H_
#define RGBLED_H_

#include "MKL25Z4.h"

#define RED_LED_SHIFT 3 	//PTB3
#define BLUE_LED_SHIFT 2	//PTB2
#define GREEN_LED_SHIFT 1	//PTB1


/* Actions to set colours */
#define RED_ON		GPIOB->PCOR = (1UL << RED_LED_SHIFT)  	//Clear register
#define RED_OFF		GPIOB->PSOR = (1UL << RED_LED_SHIFT)	//Set register

#define GREEN_ON	GPIOB->PCOR = (1UL << GREEN_LED_SHIFT)  //Clear register
#define GREEN_OFF	GPIOB->PSOR = (1UL << GREEN_LED_SHIFT)	//Set register

#define BLUE_ON		GPIOB->PCOR = (1UL << BLUE_LED_SHIFT)  	//Clear register, notice port D
#define BLUE_OFF	GPIOB->PSOR = (1UL << BLUE_LED_SHIFT)	//Set register

/* Define a type for easy ref to the colours */
typedef enum {BLACK, WHITE, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA} rgb_color_t;


//Prototypes
void init_rgb(void);
void set_rgb(rgb_color_t col);

#endif /* RGBLED_H_ */
