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
#include "rgbLed.h"
#include "pwm.h"
#include "pwmBuzzer.h"
#include "tasklist.h"
#include "vib.h"
#include "pwmServo.h"


/* Defines the tone lengths to be played */
//volatile uint16_t totSeqRuns = 0;
volatile uint16_t remSeqRuns = 0;
volatile uint16_t * buzzSeq = 0;		// holds the sequence of tones
//volatile uint16_t buzzDelay = 0;	    // miliseconds tone length

/* Defines the vibration lengths to be played */
volatile uint16_t remVibRuns = 0;
volatile uint16_t * vibSeq = 0;			// holds the sequence of tones


typedef struct sequence {
	uint16_t seq[64];	// holds all tones in a sequence
	uint32_t len;		// number of tones in struct
	uint32_t delay;		// milliseconds tone length
	uint32_t repeat;	// consider this boolean
} sequence_t;

//sequence_t seqOk = {{La6, La3, La6}, 3 , 200, 0};
//sequence_t seqBusy = {{Mi4, Re4, Do4, Mi4, Mi4, Mi4, Mi4, 0}, 8 , 200, 0};

/* OK buzzing sequence - buzzer: Stigende skala */
sequence_t seqOk = {{261, 277, 294, 311, 330, 349, 370, 392, 415, 440}, 10, 50, 0};

/* OK vibration sequence */
sequence_t seqVibOk = {{1, 0, 1, 0}, 4, 200, 0};


/* Busy sequence */
sequence_t seqBusy = {{C3, 0, 0, 0, C3, D3, 0, 0, C3, D3, E3, 0, C3, D3, E3, F3}, 16 , 300, 1};

sequence_t seqVibBusy = {{1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0}, 16, 300, 0};


/* Super Mario :) */
/* sequence_t seqBusy = {{
		E7, E7, 0, E7,
		0, C7, E7, 0,
		G7, 0, 0, 0,
		G6, 0, 0, 0,

		C7, 0, 0, G6,
		0, 0, E6, 0,
		0, A6, 0, B6,
		0, A6s, A6, 0,

		G6, E7, G7, 0,
		A7, 0, F7, G7,
		0, E7, 0, C7,
		D7, B6, 0, 0,

		G6, E7, G7, 0,
		A7, 0, F7, G7,
		0, E7, 0, C7,
		D7, B6, 0, 0
}, 64 , 200, 1}; */

/* Ubehagelig høj-lav bippelyd */
/* sequence_t seqWarning = {{E7, E3}, 2, 200, 1}; */
sequence_t seqWarning = {{E4}, 5, 150, 1};

sequence_t seqVibWarning = {{1, 0}, 10, 80, 1};

/* tre bip, der harmonerer med LED'en */
sequence_t seqError = {{C8, 0, 0, C8, 0, 0, C8, 0, 0, 0, 0, 0, 0, 0, 0}, 24, 60, 1};

sequence_t seqVibError = {{1, 0, 0, 1, 0, 0, 1, 0, 0}, 24, 60, 1};

/* Register variables to handle sound inside state machine */
sequence_t * seq_reg = NULL;
sequence_t * seq_next = NULL;

/* Register variables to handle vibration inside state machine */
sequence_t * vib_reg = NULL;
sequence_t * vib_next = NULL;



/* Task manages switch and state machine */
void TaskHandleSwitch(void* p) {

	/* Initialize system states */
	db_state_t db_reg = zero;
	db_state_t db_next = wait1;
	sys_state_t sys_reg = entrypoint;
	sys_state_t sys_next = ok;
	rgb_color_t rgb_reg = WHITE;
	rgb_color_t rgb_next = BLACK;

	/* angle for servo */
	uint8_t servo_reg = 0;
	uint8_t servo_next = 0;

	/* initialize ticks and counters */
	uint8_t db_tick = 0;
	uint8_t q_reg, q_next;

	while(1) {

		db_tick = 0;

		/* Statemachine for debounce */

		switch (db_reg) {
		case zero:

			if (SWITCH) {
				db_next = wait1;
				q_next = db_MS_WAIT; 	// load value
			}
			break;

		case wait1:

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

			if (!SWITCH) {				// if switch no longer pressed
				db_next = wait0;
				q_next = db_MS_WAIT;	// load value
			}
			break;

		case wait0:

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
				seq_next = &seqOk;

				vib_next = &seqVibOk;

				rgb_next = GREEN;

				servo_next = SERVO_OK;

				break;

			case ok:
				sys_next = busy;
				vTaskSuspend(hdlFlashOK);
				vTaskResume(hdlFlashBusy);
				seq_next = &seqBusy;

				vib_next = &seqVibBusy;

				rgb_next = BLUE;

				servo_next = SERVO_BUSY;

				break;

			case busy:
				sys_next = warning;
				vTaskSuspend(hdlFlashBusy);
				vTaskResume(hdlFlashWarning);
				seq_next = &seqWarning;

				vib_next = &seqVibWarning;

				rgb_next = YELLOW;

				servo_next = SERVO_WARNING;

				break;

			case warning:
				sys_next = error;
				vTaskSuspend(hdlFlashWarning);
				vTaskResume(hdlFlashError);
				seq_next = &seqError;

				vib_next = &seqVibError;

				rgb_next = RED;

				servo_next = SERVO_ERROR;

				break;

			case error:
				sys_next = ok;
				vTaskSuspend(hdlFlashError);
				vTaskResume(hdlFlashOK);
				seq_next = &seqOk;

				vib_next = &seqVibOk;

				rgb_next = GREEN;

				servo_next = SERVO_OK;

				break;
			}

			/* handle sys state registry */
			sys_reg = sys_next;
			seq_reg = seq_next;	/* Next sequence for the buzzing pattern */
			vib_reg = vib_next;	/* Next sequence for the vibration pattern */

			/* Handle buzzing sound state reg */
			remSeqRuns = seq_reg->len;
			vTaskResume(hdlBuzzSeq);	// start the buzzing task

			/* Handle buzzing sound state reg */
			remVibRuns = vib_reg->len;
			vTaskResume(hdlVibSeq);		// start the vibration task

			/* Handle LED color state reg */
			rgb_reg = rgb_next;
			set_rgb(rgb_reg);

			/* Handle servo state */
			servo_reg = servo_next;
			servoSetAngle(servo_reg);

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

/* Run the active buzzing sequence */
void TaskBuzzSeq(void * p) {

	uint16_t idx = 0;

	while (1) {

		if ( remSeqRuns > 0 ) {

			// still something left to play

			idx = ( seq_reg->len - remSeqRuns );    // index for sequence array
			remSeqRuns--;						// decrement remaining

			buzzTone( *(seq_reg->seq + idx) );	//pick out the tone to play, and play it

			vTaskDelay( pdMS_TO_TICKS( seq_reg->delay ) );

		} else if (seq_reg->repeat) {
			// if this is a tone to be repeated
			remSeqRuns = seq_reg->len; // let it start again

		} else {
			buzzStop();	//play nothing
			vTaskSuspend(NULL);
		}


	}

	vTaskDelete(NULL);
}



/* Run the vibration sequence */
void TaskVibSeq(void * p) {

	uint16_t idx = 0;

	while (1) {

		if ( remVibRuns > 0 ) {

			// still something left to play

			idx = ( vib_reg->len - remVibRuns );    // index for sequence array
			remVibRuns--;							// decrement remaining

			if (*(vib_reg->seq + idx)) {
				VIB_ON;
				vTaskDelay( pdMS_TO_TICKS( vib_reg->delay ) );
				VIB_OFF;
				vTaskDelay( pdMS_TO_TICKS( vib_reg->delay ) );
			} else {
				vTaskDelay( pdMS_TO_TICKS( vib_reg->delay ) );
			}


		} else if (vib_reg->repeat) {
			// if this is a tone to be repeated
			remVibRuns = vib_reg->len; // let it start again

		} else {
			vTaskSuspend(NULL);
		}


	}

	vTaskDelete(NULL);
}