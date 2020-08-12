/*
 * vib.c
 *
 *  Created on: 16 Mar 2020
 *      Author: janusboandersen
 */


#include "vib.h"

#define MASK(x)		(1UL << x)

void vib_init(void) {
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[VIB_POS] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[VIB_POS] |= PORT_PCR_MUX(1);
	PTB->PDDR |= MASK(VIB_POS);

	/* Start as off (inactive high) */
	GPIOB->PSOR = (1UL << VIB_POS);
}
