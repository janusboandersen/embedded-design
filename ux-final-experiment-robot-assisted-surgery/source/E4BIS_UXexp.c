
/**
 * @file    E4BIS_design3.c
 * @brief   Implements design for switching between states using: Switching, buzzer, RGB led, PWM
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include "FreeRTOS.h"
#include "task.h"

/* Own include files */
#include "tasklist.h"
#include "pwm.h"
#include "pwmBuzzer.h"
#include "switch.h"
#include "rgbLed.h"
#include "vib.h"
#include "pwmServo.h"
#include "adc.h"

/* Create global handles for a task */
TaskHandle_t hdlHandleSwitch = NULL;
TaskHandle_t hdlFlashOK = NULL;
TaskHandle_t hdlFlashBusy = NULL;
TaskHandle_t hdlFlashWarning = NULL;
TaskHandle_t hdlFlashError = NULL;
TaskHandle_t hdlBuzzSeq = NULL;
TaskHandle_t hdlVibSeq = NULL;
TaskHandle_t hdlFlexSens = NULL;

int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    BOARD_InitDebugConsole();   	/* Init FSL debug console. */

    /* Initialize all UX stuff */

    /* RGB diodes GPIO*/
    init_rgb();

    /* RGB diodes PWM */
    pwmInit(PWM_PERIOD);

    /* Vibration motor */
    vib_init();

    /* Piezo buzzer*/
    buzzerInit();
    //buzzTone(La3s);
    buzzStop();

    /* Switch */
    init_sw();

    /* Servo motor */
    servoInit(SERVO_PERIOD);
    servoSetAngle(SERVO_INIT);

    /* ADC for reading flex sensor */
    Init_ADC();

    PRINTF("E4BIS UX Experiment.\n");

    set_rgb(WHITE);

	/* Task for managing switch and state machines */
	xTaskCreate(TaskHandleSwitch, "FSM task", configMINIMAL_STACK_SIZE*2, NULL,	tskIDLE_PRIORITY+1UL, &hdlHandleSwitch);

	/* Tasks for displaying states */
	xTaskCreate(TaskFlashOK, "OK task", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY+1UL, &hdlFlashOK);
	xTaskCreate(TaskFlashBusy, "Busy task", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY+1UL, &hdlFlashBusy);
	xTaskCreate(TaskFlashWarning, "Warning task", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY+1UL, &hdlFlashWarning);
	xTaskCreate(TaskFlashError, "Error task", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY+1UL, &hdlFlashError);

	/*  */
	xTaskCreate(TaskBuzzSeq, "Buzz task", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY+1UL, &hdlBuzzSeq);
	xTaskCreate(TaskVibSeq, "Vibration task", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY+1UL, &hdlVibSeq);
	xTaskCreate(TaskFlexSens, "FlexSense task", configMINIMAL_STACK_SIZE*4, NULL, tskIDLE_PRIORITY+1UL, &hdlFlexSens);

	/* Set tasks as suspended before start */
	vTaskSuspend(hdlFlashOK);
	vTaskSuspend(hdlFlashBusy);
	vTaskSuspend(hdlFlashWarning);
	vTaskSuspend(hdlFlashError);
	vTaskSuspend(hdlBuzzSeq);
	vTaskSuspend(hdlVibSeq);

	/* Run the scheduler */
	vTaskStartScheduler();

    /* Enter an infinite loop - will never reach here unless RTOS broken */
    volatile static int i = 0 ;
    while(1) {
        i++ ;
        __asm volatile ("nop");
    }
    return 0 ;
}
