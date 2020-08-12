
#include "adc.h"

#define ADC_POS (20) // PTE20

int Init_ADC(void) {

	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK; //Page 173 Listing 6.5
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	// Analog for pin
	PORTE->PCR[ADC_POS] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[ADC_POS] |= PORT_PCR_MUX(0);		// analog option


	/* Low power config, long sample time, 16 Bit SE, Bus clk input*/
	ADC0->CFG1 = ADC_CFG1_ADLPC_MASK | ADC_CFG1_ADLSMP_MASK | ADC_CFG1_MODE(3) | ADC_CFG1_ADICLK(0); //page 173, listing 6.5

	/* Software trigger, compare func. disabled, DMA dsabled, Voltgae ref - VREFH and VREFL */
	ADC0->SC2 = ADC_SC2_REFSEL(0); //page 173 book Listing 6.5

	return 1;
}

// Reads from channel 0
uint16_t Read_ADC(void) {

	uint16_t n;

	ADC0->SC1[0] = 0x00;

	/* Polled implementation, wait for COCO (COnversion COmplete) bit to be set */
	while(!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
		;

	// Read result
	n = ADC0->R[0];

	return n;

}







