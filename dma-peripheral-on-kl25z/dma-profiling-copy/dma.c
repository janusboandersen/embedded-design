/*
 * @file	dma.c
 * @author	Janus Bo Andersen
 * @date	January 2020
 * @brief	Implements initialization and functions for listing 9.3
 *
 */

#include "dma.h"

uint32_t s[ARR_SIZE], d[ARR_SIZE]; //source and destination arrays

/*
 * @brief	software data copy, toggles PTB1 high while copying
 */
void test_sw_copy(void) {
	uint32_t *ps, *pd;

	uint16_t i; // for counting up array index

	// Initialize array with some data
	for (i = 0; i < ARR_SIZE; i++) {
		s[i] = i;	//source initialized to some number
		d[i] = 0;	//destination initialized to zero
	}

	TOGGLE_DEBUG_PIN();		//toggle high while copying

	ps = s; // set pointers equal to first elem in arrays
	pd = d;

	for (int i = 0; i < ARR_SIZE; i++) {
		*pd++ = *ps++;	// increment ptr, dereference value, copy
	}

	TOGGLE_DEBUG_PIN();		//toggle low when done copying
}

/*
 * @brief	initalize pin for measuring speed
 */
void init_dbg_pin(void) {

	//TODO: Use FGPIO
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; 	// Enable clk-gating to PTB
	PORTB->PCR[DBG_PIN] |= PORT_PCR_MUX(1);	// Enable GPIO function
	PTB->PDDR |= MASK(DBG_PIN);				// 1 for output

	//Initialize low, to be sure
	PTB->PCOR |= MASK(DBG_PIN); 			// Clear

}

/*
 * @brief	initialize DMA to copy 32bit->32bit and increment addresses.
 * DMA Mux is not enabled.
 */
void init_dma(void) {

	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;		// enable clock for DMA
	DMA0->DMA[0].DCR = DMA_DCR_SINC_MASK |	// increment source addr
					  DMA_DCR_SSIZE(0) |	// source data size is 32-bit
					  DMA_DCR_DINC_MASK |	// increment dest addr
					  DMA_DCR_DSIZE(0);		// dest data size is 32-bit
}

/*
 * @brief	Copy data words via DMA channel 0, 32-bit words
 * @param	source	source address
 * @param 	dest	destination address
 * @param	count	number of words to copy
 */
void copy_32bit(uint32_t * source, uint32_t * dest, uint32_t count) {

	// Set source and destination memory addresses in SAR and DAR
	DMA0->DMA[0].SAR = DMA_SAR_SAR( (uint32_t) source );	//cast source addr ptr to 32 bit num
	DMA0->DMA[0].DAR = DMA_DAR_DAR( (uint32_t) dest );		//cast dest addr ptr to 32 bit num

	// Init byte count register (BCR), 4 bytes per word to copy (32-bit)
	DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(count * 4);

	// Clear DONE flag
	DMA0->DMA[0].DSR_BCR &= ~DMA_DSR_BCR_DONE_MASK;			// Must be zero in order to start

	TOGGLE_DEBUG_PIN(); //Set DBG PIN high during transfer

	// Software start of transfer
	DMA0->DMA[0].DCR |= DMA_DCR_START_MASK;					// Write 1 to START to begin

	//Busy polling until DONE goes high
	while ( !(DMA0->DMA[0].DSR_BCR & DMA_DSR_BCR_DONE_MASK ) ) {
		// Zzzz!
	}

	TOGGLE_DEBUG_PIN(); //Set DBG PIN low as transfer is done
}

/*
 * @brief	wrapper function to test the DMA copy
 */
void test_dma_copy(void) {

	uint16_t i; // for counting up array index

	// Initialize array with some data
	for (i = 0; i < ARR_SIZE; i++) {
		s[i] = i;	//source initialized to some number
		d[i] = 0;	//destination initialized to zero
	}

	// Begin to copy from s to d
	copy_32bit(s, d, ARR_SIZE);
}
