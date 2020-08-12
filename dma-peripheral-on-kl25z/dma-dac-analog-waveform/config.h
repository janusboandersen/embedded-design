/*
 * @file	config.h
 * @author	Janus Bo Andersen
 * @date	Feb 2020
 * @brief	General config and settings for ex.2 wk5/6
 *
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "MKL25Z4.h"

#define MASK(x) (1UL << x)

#define BLUE_LED_POS (1) 								// PTD1
#define DBG_PIN (1)										// Use PTB1 for measuring time
#define TOGGLE_DEBUG_PIN() FPTB->PTOR = 1UL << DBG_PIN	// Macro to Fast toggle PTB1

#define DMA_ISR_PIN (0)									// Use PTB0 for measuring time
#define TOGGLE_DMA_ISR_PIN() FPTB->PTOR = 1UL << DMA_ISR_PIN // Macro to Fast toggle PTB1

#define DMA_COPY 1				// 1 for yes, 0 for no

#define TPM_PERIOD 10			// 10 us

#define DAC_POS (30)			// PTE30
#define DAC_RESOLUTION (4096)	// DAC code up to...
#define STEP_SIZE (16) 			// Step size for DAC code
#define NUM_STEPS (512)			// DAC_RESOLUTION / STEP_SIZE

extern uint16_t triangle_data[NUM_STEPS]; //defined in dac.c

#endif /* CONFIG_H_ */
