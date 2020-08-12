
#include "pwm.h"

#define MASK(x) (1UL << x)


int pwmInit(uint16_t period) {

	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;	// Clock PTC2 (YELLOW LED)
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;	// Clock TPM

	//Let's use FAST GPIO
	PORTC->PCR[YELLOW_LED_POS] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[YELLOW_LED_POS] |= PORT_PCR_MUX(4); 	//Alt. 4 FlexTimer Ch 1

	//Config TPM0
	SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK); //48 MHz
	TPM0->MOD = period-1; // MOD counter mod
	TPM0->SC = TPM_SC_PS(1); //count up, div by 2 prescaler
	TPM0->CONF |= TPM_CONF_DBGMODE(3); //Let it work in debug mode
	TPM0->CONTROLS[1].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK; //Low-true and edge align
	TPM0->CONTROLS[1].CnV = 0; //initial duty cycle
	TPM0->SC |= TPM_SC_CMOD(1);	//START :)

	return 0;
}

void init_dbg_pin(void) {

	//SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; 	// Enable clk-gating to PTB
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK; 	// Enable clk-gating to PTD

	//PTD1: Blue LED
	PORTD->PCR[BLUE_LED_POS] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED_POS] |= PORT_PCR_MUX(1);	// GPIO
	PTD->PDDR |= MASK(BLUE_LED_POS);				// 1 for output

	//PTB1: dbg for ticks
	//PORTB->PCR[DBG_PIN] |= PORT_PCR_MUX(1);	// Enable GPIO function
	//PTB->PDDR |= MASK(DBG_PIN);				// 1 for output

	//PTB0: dbg for DMA0 ISR
	//PORTB->PCR[DMA_ISR_PIN] |= PORT_PCR_MUX(1);	// Enable GPIO function
	//PTB->PDDR |= MASK(DMA_ISR_PIN);				// 1 for output

	//Initialize off, to be sure
	PTD->PSOR |= MASK(BLUE_LED_POS); 	// Clear - active low
	//PTB->PCOR |= MASK(DBG_PIN); 		// Clear - active high
	//PTB->PCOR |= MASK(DMA_ISR_PIN);	// Clear
}
