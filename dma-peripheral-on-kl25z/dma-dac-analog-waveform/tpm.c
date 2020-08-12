/*
 * @file	tpm.c
 * @author	Janus Bo Andersen
 * @date	Feb 2020
 * @brief	Implementation of TPM init and IRQHandler for ex.2 wk5/6
 *
 */

#include "tpm.h"

void init_dbg_pin(void) {

	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; 	// Enable clk-gating to PTB

	//PTB1: dbg for TPM0 ISR
	PORTB->PCR[DBG_PIN] |= PORT_PCR_MUX(1);	// Enable GPIO function
	PTB->PDDR |= MASK(DBG_PIN);				// 1 for output

	//PTB0: dbg for DMA0 ISR
	PORTB->PCR[DMA_ISR_PIN] |= PORT_PCR_MUX(1);	// Enable GPIO function
	PTB->PDDR |= MASK(DMA_ISR_PIN);				// 1 for output

	//Initialize low, to be sure
	PTB->PCOR |= MASK(DBG_PIN); 			// Clear
	PTB->PCOR |= MASK(DMA_ISR_PIN);			// Clear

}


/*
 * @brief	This function initializes the TPM w/o IRQ, assumed at 48 MHz internal TPM clock
 */
void init_tpm_wo_irq(uint32_t period_us) {

	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK; 		// en clock for TPM0
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1) | 	// sel FLL or PLL/2 clock from multipurp. clk gen
				 SIM_SOPT2_PLLFLLSEL_MASK; 	// sel the PLL / 2 clock

	TPM0->SC = 0UL; //Off and clear before config

	// LDV = round(T_target * f_count - 1) =>
	// LDV = round(T_target / T_counter - 1) =>
	// LDV = round(F_counter / F_target - 1)
	TPM0->MOD = TPM_MOD_MOD(24*period_us);	//24 MHz due to div-by-2 prescaler

	TPM0->SC = TPM_SC_DMA_MASK | // enable DMA flag, man. p. 552
			   TPM_SC_PS(1);	 // prescale by 2, man. p. 553
}

/*
 * @brief	This function initializes the TPM w/IRQ, assumed at 48 MHz internal TPM clock
 */
void init_tpm_w_irq(uint32_t period_us) {

	init_tpm_wo_irq(period_us);

	TPM0->SC |= TPM_SC_TOIE_MASK; // enable interrupts

	//Set up NVIC
	NVIC_SetPriority(TPM0_IRQn, 128); //low prio
	NVIC_ClearPendingIRQ(TPM0_IRQn);
	NVIC_EnableIRQ(TPM0_IRQn);
}

void start_tpm() {
	TPM0->SC |= TPM_SC_CMOD(1);	// en by using TPM internal clock
}

void stop_tpm() {
	TPM0->SC &= ~TPM_SC_CMOD_MASK;	// disable by no LPTPM counter, man. p. 553
}

/*
 * @brief	ISR for TPM0 to output via DAC
 */
void TPM0_IRQHandler() {
	static int change = STEP_SIZE;
	static uint16_t out_data = 0;

	TOGGLE_DEBUG_PIN();

	// Clear the overflow flag by writing 1, man. p. 552
	TPM0->SC |= TPM_SC_TOF_MASK;
	NVIC_ClearPendingIRQ(TPM0_IRQn);

	// ISR work here

	// Configure the signal change per round
	out_data += change;
	if ( out_data < STEP_SIZE ) {
		change = STEP_SIZE;
	} else if (out_data >= DAC_RESOLUTION - STEP_SIZE) {
		change = -STEP_SIZE;
	}

	// Output via the DAC, high bits first, then low bits
	DAC0->DAT[0].DATH = DAC_DATH_DATA1(out_data >> 8);
	DAC0->DAT[0].DATL = DAC_DATL_DATA0(out_data);

	TOGGLE_DEBUG_PIN();
}
