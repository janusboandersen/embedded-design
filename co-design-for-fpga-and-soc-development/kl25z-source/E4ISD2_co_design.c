 
/**
 * @file    E4ISD2_wk06_FreeRTOS_test.c
 * @brief   Application entry point.
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
#include "queue.h"

#include "spi.h"

/* Data structure for data transfer */
typedef struct {
	uint8_t addr; 	// 1 byte address field
	uint8_t data;	// 1 byte value field (0-255)
} fpga_cmd_t;

/* Queue for SPI commands */
#define QUEUE_LEN 4												// Queue depth
QueueHandle_t spi_queue_hdl = NULL; 							// Create handle for the queue

/* Create handle for the tasks */
TaskHandle_t menu_hdl = NULL;
TaskHandle_t spi_tx_hdl = NULL;


/* The SPI TX tasks receives commands from a queue
 * and sends the command in 8 bit words (bytes) over the SPI interface.
 * */
void spi_tx(void * p) {

	fpga_cmd_t send_me;

	while(1) {

		// Receive from queue
		if ( xQueueReceive(spi_queue_hdl, &send_me, 500) ) {

			// Send first part of command (address)
			SPIsend(send_me.addr);

			// Send second part of command (value)
			SPIsend(send_me.data);

			printf("Sent over SPI.\n");

		} else {
			// failed to receive from queue

		}

		vTaskDelay(pdMS_TO_TICKS(10)); //delay for 100 ms
	}

	//Remember to call delete on this task in case it somehow breaks the loop
	vTaskDelete(NULL);
}

uint8_t getbyte(void) {

	unsigned int buf;
	char c;

	// Get one unsigned integer
	scanf("%u", &buf);

	// Remove all trailing chars
	while ((c = getchar()) != '\n' && c != EOF)
		;

	return buf;
}


/* The menu task gets the user input, packs it up in a datastructure
 * and sends it in the SPI queue.
 * */
void menu(void * p) {

	fpga_cmd_t send_me;
	uint8_t buf;

	while(1) {

		// Get LED from user
		printf("Enter LED number (address) 0-11:\n");
		buf = getbyte();
		send_me.addr = buf;
		printf("Entered value: %u\n", buf);

		// Get PWM value from user
		printf("Enter PWM value 0-255:\n");
		buf = getbyte();
		send_me.data = buf;
		printf("Entered value: %u\n", buf);

		// Send by putting in the queue
		printf("Sending.\n");

		xQueueSend(spi_queue_hdl, &send_me, 500);

		vTaskDelay(pdMS_TO_TICKS(10)); //delay for 100 ms
	}

	//Remember to call delete on this task in case it somehow breaks the loop
	vTaskDelete(NULL);


}


int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    PRINTF("Hello. Co-design Spring 2020.\n");

    Init_SPI1();

    /* Create the SPI queue */
    spi_queue_hdl = xQueueCreate(QUEUE_LEN, sizeof(fpga_cmd_t) );

    /* Create the SPI TX task */
    xTaskCreate(spi_tx, "SPI TX", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY+1, &spi_tx_hdl);

    /* Create the menu task */
    xTaskCreate(menu, "Menu", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY+1, &menu_hdl);

    /* Run the scheduler */
    vTaskStartScheduler();

    /* We should never reach here... */
	volatile static int i = 0 ;
	while(1) {
		i++ ;
	}

	return 0 ;
}
