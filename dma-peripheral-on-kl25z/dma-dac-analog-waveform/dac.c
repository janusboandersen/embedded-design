/*
 * @file	dac.h
 * @author	Janus Bo Andersen
 * @date	Feb 2020
 * @brief	Implementation of init and functions for DAC for ex.2 week 5/6
 *
 */

#include "dac.h"

uint16_t triangle_data[NUM_STEPS];

/*
 * @brief	Initialize DAC for output on PTE30
 */
void init_dac(void) {

	//init see book p. 159
	SIM->SCGC6 |= SIM_SCGC6_DAC0_MASK;  //en clk for DAC
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; //en clk for gpio output

	PORTE->PCR[DAC_POS] &= ~PORT_PCR_MUX_MASK;	//clear
	PORTE->PCR[DAC_POS] |= PORT_PCR_MUX(0);		//select analog for pin

	// Disable buffer mode
	DAC0->C1 = 0;
	DAC0->C2 = 0;

	// Enable DAC, select VDDA as reference voltage
	DAC0->C0 = DAC_C0_DACEN_MASK | DAC_C0_DACRFS_MASK;
}

/*
 * @brief	Build table to hold triangle signal data
 */
void init_triangle_data() {
	unsigned int n, sample;

	//build triangle signal from front and back
	for (n = 0; n < NUM_STEPS/2; n++) {
		sample = ( n * DAC_RESOLUTION / (NUM_STEPS / 2) );
		triangle_data[n] = sample;
		triangle_data[NUM_STEPS-(n+1)] = sample;
	}
}
