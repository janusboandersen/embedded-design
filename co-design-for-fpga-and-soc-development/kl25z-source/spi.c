
#include "spi.h"
#include <stdio.h>

void Init_SPI1(void) {

	/* Enable clocks to SPI1 and PTE */
	SIM->SCGC4 |= SIM_SCGC4_SPI1_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	/* Follow initialization sequence
	 * Dean, p. 225 or manual p 683 */

	/* Disable SPI1, clear SPE */
	SPI1->C1 &= ~SPI_C1_SPIE_MASK;

	/* Pin setup for SCK, MOSI, MISO, CS */

	PORTE->PCR[SPI1_MISO_POS] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[SPI1_MISO_POS] |= PORT_PCR_MUX(5);	//PTE1 alt. 5 MISO

	PORTE->PCR[SPI1_SCLK_POS] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[SPI1_SCLK_POS] |= PORT_PCR_MUX(2);	//PTE2 alt. 2 SCK

	PORTE->PCR[SPI1_MOSI_POS] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[SPI1_MOSI_POS] |= PORT_PCR_MUX(5);	//PTE3 alt. 5 MOSI

	PORTE->PCR[SPI1_PCS0_POS] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[SPI1_PCS0_POS] |= PORT_PCR_MUX(2);	//PTE4 alt. 2 PCS0 (SS)

	/* Master mode with auto SS, see man. p. 662 */
	SPI1->C1 = SPI_C1_MSTR_MASK | SPI_C1_SSOE_MASK;
	SPI1->C2 = SPI_C2_MODFEN_MASK;

	// Mode 0
	SPI1->C1 &= ~SPI_C1_CPHA_MASK;
	SPI1->C1 &= ~SPI_C1_CPOL_MASK; // active low

	/* Set baud rate at 48 MHz/ ( (SPPR+1) x 2^(SPR+1) )
	 * 48 MHz / (8 * 2^(7) ) = 46.9 kHz */
	SPI1->BR = SPI_BR_SPPR(7) | SPI_BR_SPR(6);

	/* Good to go, enable SPI1 */
	SPI1->C1 |= SPI_C1_SPE_MASK;

}

/* @brief Send and receive from Master
 * using polling method
 * */
uint8_t SPIsend(uint8_t d_out) {

	/* Transmit byte (must follow sequence of book. p.224 / man.p.667)
	 * SPTEF is high when transmit buffer is empty */
	while ( !(SPI1->S & SPI_S_SPTEF_MASK) ) {
		//idling until ready
	}

	/* Write data to transmit register, to be sent immediately */
	SPI1->D = d_out;

	/* Receive byte
	 * SPRF is high when receive buffer is full */
	while ( !(SPI1->S & SPI_S_SPRF_MASK) ) {
		// idling until ready
	}

	/* Read any received value and return it */
	return SPI1->D;
}



void Test_SPI_Loopback(void) {
	uint8_t out = 'A';
	uint8_t in;

	while (1) {
		in = Test_SPIsend(out);

		/* confirm in == out */
		if (in != out) {
			/* Error, transmit doesnt match */
			printf("Transfer failed.\n");
		} else {
			//printf("Transfer OK.\n");
		}

		/* next character */
		out++;
		if (out > 'z') {
			/* Start all over */
			out = 'A';
		}
	}
}
