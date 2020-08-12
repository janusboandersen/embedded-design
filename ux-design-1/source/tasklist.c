/*
 * tasklist.c
 *
 *  Created on: 16 Feb 2020
 *      Author: janusboandersen
 */

#include <stdio.h>

#include "MKL25Z4.h"

#include "FreeRTOS.h"
#include "task.h"

#include "switch.h"
#include "pwm.h"
#include "tasklist.h"


/* counts cycles in the application idle hook callback */
//volatile uint32_t ulIdleCycleCount = 0UL;

/* Task manages switch and state machine */
void TaskHandleSwitch(void* p) {

	/* Initialize system states */
	db_state_t db_reg = zero;
	db_state_t db_next = wait1;
	sys_state_t sys_reg = entrypoint;
	sys_state_t sys_next = ok;

	/* initialize ticks and counters */
	uint8_t db_tick = 0;
	uint8_t db_level = 0;
	uint8_t q_reg, q_next;

	while(1) {

		db_tick = 0;

		/* Statemachine for debounce */

		switch (db_reg) {
		case zero:
			db_level = 0;

			if (SWITCH) {
				db_next = wait1;
				q_next = db_MS_WAIT; 	// load value
			}
			break;

		case wait1:
			db_level = 0;

			if (SWITCH) {
				q_next = q_reg - 1;

				if (q_next == 0) {
					db_next = one;
					db_tick = 1;
				}
			} else {
				db_next = zero; 		// SWITCH not pressed anymore
			}
			break;

		case one:
			db_level = 1;

			if (!SWITCH) {				// if switch no longer pressed
				db_next = wait0;
				q_next = db_MS_WAIT;	// load value
			}
			break;

		case wait0:
			db_level = 1;

			if (!SWITCH) {				// switch not pressed
				q_next = q_reg - 1;

				if (q_next == 0) {
					db_next = zero;
				}
			} else {
				db_next = one;			// switch pressed again
			}
			break;
		}

		/* handle registry */
		db_reg = db_next;
		q_reg = q_next;


		/* State Machine for system state */
		/* only runs on successful debounced button press */
		if (db_tick) {

			//For debugging only:
			//printf("Switch pressed and debounced!\n");
			//TOGGLE_BLUE_LED();

			switch (sys_reg) {
			case entrypoint:
				sys_next = ok;
				vTaskResume(hdlFlashOK);
				break;

			case ok:
				sys_next = busy;
				vTaskSuspend(hdlFlashOK);
				vTaskResume(hdlFlashBusy);
				break;

			case busy:
				sys_next = warning;
				vTaskSuspend(hdlFlashBusy);
				vTaskResume(hdlFlashWarning);
				break;

			case warning:
				sys_next = error;
				vTaskSuspend(hdlFlashWarning);
				vTaskResume(hdlFlashError);
				break;

			case error:
				sys_next = ok;
				vTaskSuspend(hdlFlashError);
				vTaskResume(hdlFlashOK);
				break;
			}

			/* handle registry */
			sys_reg = sys_next;

		}

		vTaskDelay(pdMS_TO_TICKS(1));
	}

	vTaskDelete(NULL);
}


void TaskFlashOK(void * p) {

    uint16_t i=0;

	while(1) {

		// Set fully off
		//TPM0->CONTROLS[1].CnV = PWM_PERIOD/4;

		//Fade in:
		for (i=(PWM_PERIOD*0.95)-1; i>PWM_PERIOD/5; i--) {
			TPM0->CONTROLS[1].CnV = i;

			//wait a while
			vTaskDelay(pdMS_TO_TICKS(FlashOKDelayMS));
		}

    	//Fade out: at PWM_PERIOD value, the LED is fully off
    	for (i=PWM_PERIOD/5; i< (PWM_PERIOD*0.95); i++) {
    		TPM0->CONTROLS[1].CnV = i;

    		//wait a while
    		vTaskDelay(pdMS_TO_TICKS(FlashOKDelayMS));
    	}

		vTaskDelay(pdMS_TO_TICKS(FlashOKLongWait));

	}

	vTaskDelete(NULL);
}


/* Busy signal */
void TaskFlashBusy(void * p) {

	uint16_t r = 0;
	srand(0);

	while (1) {
		TPM0->CONTROLS[1].CnV = 0;
		r = rand() % FlashBusyRandMax;
		vTaskDelay(pdMS_TO_TICKS(r));
		r = rand() % FlashBusyRandMax;
		TPM0->CONTROLS[1].CnV = PWM_PERIOD;
		vTaskDelay(pdMS_TO_TICKS(r));
	}

	vTaskDelete(NULL);
}

/* Warning signal: Periodic quick flashing */
void TaskFlashWarning(void * p) {

    uint16_t i=0;

	while (1) {
		TPM0->CONTROLS[1].CnV = PWM_PERIOD;	// duty cycle = CnV / MOD
		vTaskDelay(pdMS_TO_TICKS(FlashWarningDelayMS));
		TPM0->CONTROLS[1].CnV = 0;	// duty cycle = CnV / MOD
		vTaskDelay(pdMS_TO_TICKS(FlashWarningDelayMS));
	}

	vTaskDelete(NULL);
}

/* Error signal: Three quick "beeps" and a pause */
void TaskFlashError(void * p) {

	uint8_t i;

	while (1) {

		for (i = 0; i < FlashErrorNumBeeps; i++) {
			TPM0->CONTROLS[1].CnV = 0;	// duty cycle = CnV / MOD
			vTaskDelay(pdMS_TO_TICKS(FlashErrorBeepDelayMS));
			TPM0->CONTROLS[1].CnV = PWM_PERIOD;	// duty cycle = CnV / MOD
			vTaskDelay(pdMS_TO_TICKS(FlashErrorInterbeepDelayMS));
		}
		vTaskDelay(pdMS_TO_TICKS(FlashErrorDelayMS)); // long delay
	}

	vTaskDelete(NULL);
}
