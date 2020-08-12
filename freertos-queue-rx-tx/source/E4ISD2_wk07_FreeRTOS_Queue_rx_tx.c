 
/**
 * @file    E4ISD2_wk07_FreeRTOS_Queue_rx_tx.c
 * @author	Janus Bo Andersen (JA67494)
 * @date	March 2020 (E4ISD2 spring 2020)
 * @brief   Main function and tasks to implement tx-rx system
 * 			that sends CAN datagrams via a queue in FreeRTOS.
 * @note	Inspiration from https://www.youtube.com/watch?v=yHfDO_jiIFw
 */

#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define QUEUE_LEN 5			// Queue depth

/* The CAN datagram type */
typedef struct {
	unsigned int ID;		// holds 11/29 bit ID
	unsigned char ID_ext;	// set to 1 if ID is extended -29 bit, else 0
	unsigned char DLC;		// how many bytes of payload
	unsigned char data[8];	// the data payload
} can_tlg_t;

/* Create handle for the two tasks */
TaskHandle_t tx_hdl = NULL;
TaskHandle_t rx_hdl = NULL;

/* Create handle for the queue */
QueueHandle_t can_queue = NULL;


/* @brief This task transmits random CAN datagrams
 * @param None
 *  */
void tx(void * p) {

	can_tlg_t message;	// datagram

	BaseType_t rc;		// gets return code from xQueueSend

	uint8_t r = 0;		// gets the random values
	srand(0);			// seed to always start same place

	while(1) {

		/* fill the control fields */
		message.ID = 14;
		message.ID_ext = 0;
		message.DLC = 7;		// 7 bytes of data to be sent

		/* fill the data fields */
		for (int i = 0; i < message.DLC; i++) {
			r = rand() % UINT8_MAX; // draw random num
			message.data[i] = r;	// put in buffer
		}

		/* copy CAN telegram to the queue */
		/* wait for 500 ticks before timeout */
		printf("\nSent telegram with ID %d to receiver task. ", message.ID );
		rc = xQueueSend(can_queue, &message, 500);
		puts( rc ? "Sent.\n" : "Failed.\n" );

		vTaskDelay(200);
	}

	// Task must not return!
	vTaskDelete(NULL);
}

/* @brief This task receives CAN datagrams and prints them
 * @param None
 *  */
void rx(void * p) {

	can_tlg_t message;		// incoming message

	while(1) {
		/* Receive from the queue, wait max 500 ticks */
		if ( xQueueReceive(can_queue, &message, 500) ) {
			// received a message
			printf("Received CAN telegram with %d bytes of data:\n", message.DLC);
			printf("ID: %d\n", message.ID);

			for (int i = 0; i < message.DLC; i++) {
				printf("Data [%d]: %d\n", i+1, message.data[i]);
			}

		} else {
			// failed to receive, or timed out
		}
	}

	// Task must not return!
	vTaskDelete(NULL);
}


/*
 * @brief   Application entry point.
 */

int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    PRINTF("E4ISD2 FreeRTOS exercise 2/wk7. Transmission via queues.\n");

    /* Create the queue to hold 5 CAN datagrams */
    can_queue = xQueueCreate(QUEUE_LEN, sizeof(can_tlg_t) );

    if (!can_queue) {
    	printf("Error. Queue could not be created.\n");
    	return -1;	/* No point continuing */
    }


    /* Create the tx and rx tasks */
    xTaskCreate(tx,  						// function pointer to tx task
    			"TX", 						// name for debugging
				configMINIMAL_STACK_SIZE*2,	// stack size
				NULL,						// passing parameters to task
				tskIDLE_PRIORITY+1,			// task prio (higher than idle)
				&tx_hdl );					// handle to grab the task by

    xTaskCreate(rx, "RX", configMINIMAL_STACK_SIZE*2,
    			NULL, tskIDLE_PRIORITY+1, &tx_hdl );

    /* Run the scheduler */
    vTaskStartScheduler();

    /* We should never reach here... */
    volatile static int i = 0 ;
    while(1) {
        i++ ;
    }

    return 0 ;
}
