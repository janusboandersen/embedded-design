
#include "pwmServo.h"

#define MASK(x) (1UL << x)

/*  */
int servoInit(uint16_t period) {

	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;	// Clock PTA1
	SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;	// Clock TPM2, see man. p. 207

	PORTA->PCR[SERVO_POS] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[SERVO_POS] |= PORT_PCR_MUX(3); 	//Alt. 3 FTM2_CH0

	//Config TPM2 -- also see manual p. 195 (SIM_SOPT2)
	SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK); //48 MHz
	TPM2->MOD = period-1; // MOD counter mod
	TPM2->SC = TPM_SC_PS(4); //count up, div by 16 prescaler
	TPM2->CONF |= TPM_CONF_DBGMODE(3); //Let it work in debug mode

	/* Using Channel 0 */
	TPM2->CONTROLS[0].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK; //High-true and edge align
	TPM2->CONTROLS[0].CnV = 0; //initial duty cycle

	/* Engage! */
	TPM2->SC |= TPM_SC_CMOD(1);

	return 0;
}

/* @brief Set the duty cycle of the servo PWM
 * @param duty_cycle (int) 0-100 pct.
 * @return void
 */
void servoSet(int duty_cycle) {

	/* Limit to 0-100 */
	if (duty_cycle > 100) {
		duty_cycle = 100;
	} else if (duty_cycle < 0) {
		duty_cycle = 0;
	}

	/* Set the duty cycle
	 * TPM2 Ch. 0 */
	TPM2->CONTROLS[0].CnV = (uint_16t) ((float) duty_cycle * (TPM2->MOD + 1) / 100.0f);
}

