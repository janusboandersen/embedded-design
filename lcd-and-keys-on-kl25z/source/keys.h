/*
 * keys.h
 *
 *  Created on: 9 Sep 2019
 *      Author: janusboandersen
 */


/* This module defines the keys via switches and their control meaning */

#ifndef KEYS_H_
#define KEYS_H_



#define MASK(x)	(1UL << (x))

/* All switches are on port E */
#define SW_UP_POS (21)
#define SW_DN_POS (29)
#define SW_LT_POS (30)
#define SW_RT_POS (23)
#define SW_CR_POS (22)

#define NUM_KEYS (5)

typedef enum {SWUp=0, SWDn, SWLt, SWRt, SWCr} sw_t;

/* Define macro to read switches */
#define READ_SWITCHES (PTE->PDIR)

void setup_keys(void) {

	// Enable clock for GPIO keys
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; // Connect clock to Port E where switches are

	// Set 5 switches pins as GPIO
	PORTE->PCR[SW_UP_POS] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[SW_UP_POS] |= PORT_PCR_MUX(1);

	PORTE->PCR[SW_DN_POS] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[SW_DN_POS] |= PORT_PCR_MUX(1);

	PORTE->PCR[SW_LT_POS] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[SW_LT_POS] |= PORT_PCR_MUX(1);

	PORTE->PCR[SW_RT_POS] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[SW_RT_POS] |= PORT_PCR_MUX(1);

	PORTE->PCR[SW_CR_POS] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[SW_CR_POS] |= PORT_PCR_MUX(1);


	/* Clear each SW to be input */
	PTE->PDDR &= ~MASK(SW_UP_POS);
	PTE->PDDR &= ~MASK(SW_DN_POS);
	PTE->PDDR &= ~MASK(SW_LT_POS);
	PTE->PDDR &= ~MASK(SW_RT_POS);
	PTE->PDDR &= ~MASK(SW_CR_POS);
}

void test_keys(void) {
	unsigned switch_code;

	unsigned key[NUM_KEYS] = {0};

	/* Make a mask for all the switches */
	unsigned total_mask = MASK(SW_UP_POS) | MASK(SW_DN_POS) | MASK(SW_LT_POS) | MASK(SW_RT_POS) | MASK(SW_CR_POS);

	// infinite loop reading the switch values and outputting to console
	while (1) {
		// switches are active low
		switch_code = ~READ_SWITCHES; //get the 32-bit input register for PORT E and negate. Should have 1s where switches are pressed.

		// check if any switch has been pressed
		if (switch_code & total_mask) {
			//some switch has been pressed
			//printf("Some switch is pressed...\n");

			//Detect pressed keys
			key[SWUp] = switch_code & MASK(SW_UP_POS) ? 1 : 0;
			key[SWDn] = switch_code & MASK(SW_DN_POS) ? 1 : 0;
			key[SWLt] = switch_code & MASK(SW_LT_POS) ? 1 : 0;
			key[SWRt] = switch_code & MASK(SW_RT_POS) ? 1 : 0;
			key[SWCr] = switch_code & MASK(SW_CR_POS) ? 1 : 0;

			printf("Up:%u, Dn:%u, Lt:%u, Rt:%u, Cr:%u\n", key[SWUp], key[SWDn], key[SWLt], key[SWRt], key[SWCr]);

		} else {
			// no switch is pressed
			//printf("No switch is pressed...\n");
		}
	} //end while
} //end test

/* writes to the passed array */
void read_keys(unsigned key[NUM_KEYS]) {

	//unsigned total_mask = MASK(SW_UP_POS) | MASK(SW_DN_POS) | MASK(SW_LT_POS) | MASK(SW_RT_POS) | MASK(SW_CR_POS);

	// switches are active low
	unsigned switch_code = ~READ_SWITCHES; //get the 32-bit input register for PORT E and negate. Should have 1s where switches are pressed.

	//Detect pressed keys
	key[SWUp] = switch_code & MASK(SW_UP_POS) ? 1 : 0;
	key[SWDn] = switch_code & MASK(SW_DN_POS) ? 1 : 0;
	key[SWLt] = switch_code & MASK(SW_LT_POS) ? 1 : 0;
	key[SWRt] = switch_code & MASK(SW_RT_POS) ? 1 : 0;
	key[SWCr] = switch_code & MASK(SW_CR_POS) ? 1 : 0;


}


#endif /* KEYS_H_ */
