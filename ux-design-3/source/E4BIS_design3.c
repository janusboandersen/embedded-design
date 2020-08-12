
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

/* Create global handles for a task */
TaskHandle_t hdlHandleSwitch = NULL;
TaskHandle_t hdlFlashOK = NULL;
TaskHandle_t hdlFlashBusy = NULL;
TaskHandle_t hdlFlashWarning = NULL;
TaskHandle_t hdlFlashError = NULL;
TaskHandle_t hdlBuzzSeq = NULL;

int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    BOARD_InitDebugConsole();   	/* Init FSL debug console. */

    init_rgb();
    pwmInit(PWM_PERIOD);
    buzzerInit();

    //buzzTone(La3s);
    buzzStop();

    init_sw();

    PRINTF("E4BIS Design 3.\n");

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

	/* Set tasks as suspended before start */
	vTaskSuspend(hdlFlashOK);
	vTaskSuspend(hdlFlashBusy);
	vTaskSuspend(hdlFlashWarning);
	vTaskSuspend(hdlFlashError);
	vTaskSuspend(hdlBuzzSeq);

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
