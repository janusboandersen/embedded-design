/*
 * @file	dma.h
 * @author	Janus Bo Andersen
 * @date	January 2020
 * @brief	Interface for functions for listing 9.3
 *
 */


#ifndef DMA_H_
#define DMA_H_

#include "MKL25Z4.h"

#define ARR_SIZE (512)									// Size of memory copy
#define DBG_PIN (1)										// Use PTB1 for measuring time
#define MASK(x) (1UL << x)								// Masking function
#define TOGGLE_DEBUG_PIN() FPTB->PTOR = 1UL << DBG_PIN	// Macro to Fast toggle PTB1

/*
 * @brief	run software data copy
 */
void test_sw_copy(void);


/*
 * @brief	initialize pin for measuring speed
 */
void init_dbg_pin(void);

/*
 * @brief	initialize DMA
 */
void init_dma(void);

/*
 * @brief	Copy data words via DMA, 32-bit words
 * @param	source	source address
 * @param 	dest	destination address
 * @param	count	number of words to copy
 */
void copy_32bit(uint32_t * source, uint32_t * dest, uint32_t count);

/*
 * @brief	wrapper function to test the DMA copy
 */
void test_dma_copy(void);


#endif /* DMA_H_ */
