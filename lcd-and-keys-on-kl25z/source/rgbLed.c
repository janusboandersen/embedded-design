/*
 * rgbLed.c
 *
 *  Created on: 9 Sep 2019
 *      Author: janusboandersen
 */

#include "pin_mux.h"
#include "clock_config.h"
#include "rgbLed.h"

#define MASK(x) (1UL << (x))

void init_rgb(void)
{
	/* Set up GPIO by enabling clocks (see slide 16) */
	/* Enable clocks to ports B and D */
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK; // Port doesn't work without clock signal (p.33)

	/* 3 GPIO pins, connect pads to GPIO via MUX (slide 17) */
	PORTB->PCR[RED_LED_SHIFT] &= ~PORT_PCR_MUX_MASK; //Bit 18 of Port B is set to MUX function  (disable everything but possible 111 in the three MUX bits)
	PORTB->PCR[RED_LED_SHIFT] |= PORT_PCR_MUX(1); //Choose Alternative 1 to connect GPIO via the mux (p. 35-36)

	PORTB->PCR[GREEN_LED_SHIFT] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED_SHIFT] |= PORT_PCR_MUX(1);

	PORTD->PCR[BLUE_LED_SHIFT] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED_SHIFT] |= PORT_PCR_MUX(1);

	/* Set data direction as outputs */
	PTB->PDDR |= MASK(RED_LED_SHIFT) | MASK(GREEN_LED_SHIFT); 	//Port B
	PTD->PDDR |= MASK(BLUE_LED_SHIFT); 							//Port D
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

/* Control on-board RGB via calling by a triple (r,g,b) */
void control_rgb(unsigned red_on, unsigned green_on, unsigned blue_on) {

	if (red_on) {
		RED_ON;
	} else {
		RED_OFF;
	}

	if (green_on) {
		GREEN_ON;
	} else {
		GREEN_OFF;
	}

	if (blue_on) {
		BLUE_ON;
	} else {
		BLUE_OFF;
	}

}
