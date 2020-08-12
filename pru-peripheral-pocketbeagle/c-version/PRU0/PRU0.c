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
		// Read the state of the input in GPIO
		gpio_value = __R31 & MASK(SWITCH_POS);	//gpio_value will be 1 or 0

		//communicate this to PRU1 via shared mem
		shared[0] = gpio_value; // will be 1 or zero
	}
}

