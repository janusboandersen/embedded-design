/*
 * rgbLed.c
 *
 *  Created on: 25 Feb 2020
 *      Author: janusboandersen
 */

#include "pin_mux.h"
#include "clock_config.h"
#include "rgbLed.h"

#define MASK(x) (1UL << (x))

void init_rgb(void)
{
	/* Set up GPIO by enabling clocks (see slide 16) */
	// Port doesn't work without clock signal (p.33)
	/* Enable clocks to ports B */
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;

	/* 3 GPIO pins, connect pads to GPIO via MUX (slide 17) */
	PORTB->PCR[RED_LED_SHIFT] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED_SHIFT] |= PORT_PCR_MUX(1);

	PORTB->PCR[GREEN_LED_SHIFT] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED_SHIFT] |= PORT_PCR_MUX(1);

	PORTB->PCR[BLUE_LED_SHIFT] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[BLUE_LED_SHIFT] |= PORT_PCR_MUX(1);

	/* Set data direction as outputs */
	PTB->PDDR |= MASK(RED_LED_SHIFT) | MASK(GREEN_LED_SHIFT) | MASK(BLUE_LED_SHIFT); 	//Port B
}

void set_rgb(rgb_color_t col)
{

	switch(col) {
	case BLACK:
		RED_OFF;
		GREEN_OFF;
		BLUE_OFF;
		break;
	case WHITE:
		RED_ON;
		GREEN_ON;
		BLUE_ON;
		break;
	case RED:
		RED_ON;
		GREEN_OFF;
		BLUE_OFF;
		break;
	case GREEN:
		RED_OFF;
		GREEN_ON;
		BLUE_OFF;
		break;
	case BLUE:
		RED_OFF;
		GREEN_OFF;
		BLUE_ON;
		break;
	case YELLOW:
		RED_ON;
		GREEN_ON;
		BLUE_OFF;
		break;
	case CYAN:
		RED_OFF;
		GREEN_ON;
		BLUE_ON;
		break;
	case MAGENTA:
		RED_ON;
		GREEN_OFF;
		BLUE_ON;
		break;
	}
}
