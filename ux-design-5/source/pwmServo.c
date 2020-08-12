
#include "pwmServo.h"
#include <stdio.h>

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
 * @param duty_cycle (float) 0-100 pct.
 * @return void
 */
void servoSetDuty(float duty_cycle) {

	/* Limit to 0-100 */
	if (duty_cycle > 100.0f) {
		duty_cycle = 100.0f;
	} else if (duty_cycle < 0.0f) {
		duty_cycle = 0.0f;
	}

	/* Set the duty cycle
	 * TPM2 Ch. 0 */
	TPM2->CONTROLS[0].CnV = (uint16_t) ((float) duty_cycle * (TPM2->MOD + 1) / 100.0f);
}

/* @brief Set the ANGLE of the servo PWM
 * @param angle (uint8_t) 0-180 deg.
 * @return void
 */
void servoSetAngle(uint8_t angle) {

	/* Limit to 0-100 */
	if (angle > 180) {
		angle = 180;
	} else if (angle < 0) {
		angle = 0;
	}

	/* According to online
	 * 50 Hz, so 1 period is 20 ms
	 * 0 deg -> 1.0 ms -> 5% duty cycle
	 * 90 deg -> 1.5 ms -> 7.5% duty cycle
	 * 180 deg -> 2.0 ms -> 10% duty cycle */

	/* In reality, it is not linear :(
	 * 50 Hz, so 1 period is 20 ms
	 * 0 deg -> 1% duty cycle
	 * 180 deg -> 15% duty cycle */


	float duty_cycle = (1.0f + angle * (14.0f / 180.0f));
	servoSetDuty(duty_cycle);
}
