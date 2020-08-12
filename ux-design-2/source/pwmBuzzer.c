
#include "pwmBuzzer.h"

#define MASK(x) (1UL << x)


/* Inits the buzzer so that the PWM is on, but the output is zero */
int buzzerInit(void) {

	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;	// Clock PTA13 (Buzzer driver)
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;	// Clock TPM

	// Set up output for buzzer driver
	PORTA->PCR[BUZZER_POS] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[BUZZER_POS] |= PORT_PCR_MUX(3); 	//Alt. 3 FlexTimer 1, Ch 1

	//Config clock source for TPMs  --- done already by other PWM
	//SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK); //48 MHz

	TPM1->MOD = 0; 						// MOD counter mod
	TPM1->SC = TPM_SC_PS(3); 			//count up, div by 8 prescaler -> counts at 6 MHz
	TPM1->CONF |= TPM_CONF_DBGMODE(3); 	//Let it work in debug mode
	TPM1->CONTROLS[1].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
	TPM1->CONTROLS[1].CnV = 0; 			//initial duty cycle
	TPM1->SC |= TPM_SC_CMOD(1);			//START :)

	return 0;
}

/* Disable tone by setting duty cycle to 0 */
void buzzStop(void) {
	TPM1->CONTROLS[1].CnV = 0;
}

void buzzTone(uint16_t freq) {

	/* f_tone = (48MHz / PRESCALER) / MOD
	 * So the idea here is to set MOD to achieve right tone,
	 * and then set CnV to get 50% duty cycle.
	 * I've hardwired the prescaler to 8.
	 */

	uint32_t tone_mod = 0;

	if (freq > 0) {
		// normal case
		tone_mod = 6000000/ (uint32_t) freq; // do casting to avoid too much integer truncation
	} else {
		// allow passing in zero to mute
		tone_mod = 1;
	}

	// Insert MOD into TPM
	TPM1->MOD = (uint16_t) tone_mod - 1; // cast to 16-bit for MOD in TPM1

	/* Ensure 50 % duty cycle */
	TPM1->CONTROLS[1].CnV = tone_mod/2;

}
