/*
 * @file	dma.c
 * @author	Janus Bo Andersen
 * @date	Feb 2020
 * @brief	Implementatio of init of DMA for ex.2 wk5/6
 *
 */

#include "dma.h"

uint16_t * dma_source = 0;
uint32_t dma_byte_count = 0;

/*
 * @brief	initialize DMA to copy 16 bit -> 16 bit and increment addresses.
 * 	DMA Mux is enabled.
 */
void init_dma(uint16_t * source, uint32_t count) {

	//Save in global variables
	dma_source = source;	//pointer to beginning of triangle array
	dma_byte_count = count * 2; //2 bytes per data item (16 bit data)

	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;		// en clk for DMA
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;	// en clk for DMAMUX

	DMAMUX0->CHCFG[0] = 0;	//man. p. 340, disable during config

	DMA0->DMA[0].DCR = DMA_DCR_SINC_MASK |	// increment source addr
					   DMA_DCR_SSIZE(2) |	// source data size is 16-bit (p. 358)
					   DMA_DCR_DSIZE(2) |	// dest data size is 16-bit (p. 359)
					   DMA_DCR_EINT_MASK |	// en interrupt (p. 357)
					   DMA_DCR_ERQ_MASK |	// en periph. request (p. 357)
					   DMA_DCR_CS_MASK; 	// cycle-steal mode (p. 358)

	//Config DMAMUX peripheral request source (man. p. 340)
	DMAMUX0->CHCFG[0] = DMAMUX_CHCFG_SOURCE(54);

	//Config NVIC
	NVIC_SetPriority(DMA0_IRQn, 2);
	NVIC_ClearPendingIRQ(DMA0_IRQn);
	NVIC_EnableIRQ(DMA0_IRQn);
}

/*
 * @brief	Sets SAR, DAR, BCR. Clears DONE and enables channel thru DMAMUX.
 */
void start_dma(void) {

	//SAR and DAR config
	DMA0->DMA[0].SAR = DMA_SAR_SAR((uint32_t) dma_source);	//from triangle_data
	DMA0->DMA[0].DAR = DMA_DAR_DAR((uint32_t) &(DAC0->DAT[0])); //to DAC data field

	DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(dma_byte_count); //bytes to be copied

	//Get ready!
	DMA0->DMA[0].DSR_BCR &= ~DMA_DSR_BCR_DONE_MASK; //clear DONE (necessary?)
	DMAMUX0->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK;	//ENBL to enable channel (p. 340)
}

/*
 * @brief	ISR for DMA0, called when copy of complete triangle period performed.
 */
void DMA0_IRQHandler(void) {
	TOGGLE_DMA_ISR_PIN();

	DMA0->DMA[0].DSR_BCR |= DMA_DSR_BCR_DONE_MASK; //write 1 to clear all status (p.356)
	start_dma(); //begin again ;)

	TOGGLE_DMA_ISR_PIN();
}
