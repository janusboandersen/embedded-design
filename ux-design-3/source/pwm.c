
#include "pwm.h"

#define MASK(x) (1UL << x)


int pwmInit(uint16_t period) {

	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;	// Clock PTC2 (YELLOW LED)
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;	// Clock TPM

	PORTC->PCR[PWM_POS] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[PWM_POS] |= PORT_PCR_MUX(4); 	//Alt. 4 FlexTimer Ch 1

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

