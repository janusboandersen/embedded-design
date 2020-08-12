#include <stdint.h>
#include <pru_cfg.h>
#include "resource_table_empty.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

#define MASK(x) (1UL << x)
#define SWITCH_POS 3		// We take GPIO input from PRU0_3
#define LED_POS 10		// We want to blink PRU1_10

void main(void)
{

	// Access to shared memory area
	volatile uint32_t * shared = (unsigned int *) 0x00010000;

	volatile uint32_t gpio_value;

	// Clear SYSCFG[STANDBY_INIT] to enable OCP master port
	CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

	while (1) {
		// Get the value that PRU0 communicated via shared memory
		gpio_value = shared[0]; // will be 1 or zero

		//set the LED appropriately
		if (gpio_value) {
			__R30 |= MASK(LED_POS);	 //gpio was high, so set LED high
		} else {
			__R30 &= ~MASK(LED_POS); //gpio was low, so set LED low
		}
	}
}

