
#include "switch.h"

void init_sw() {

	// Enable clock for GPIO
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

	// Set as GPIO input
	PORTC->PCR[SW_POS] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[SW_POS] |= PORT_PCR_MUX(1);		// GPIO function
	PTC->PDDR &= ~MASK(SW_POS);					// input direction
}
