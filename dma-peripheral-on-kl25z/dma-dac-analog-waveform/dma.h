/*
 * @file	dma.h
 * @author	Janus Bo Andersen
 * @date	Feb 2020
 * @brief	Interface for init of DMA for ex.2 wk5/6
 *
 */

#ifndef DMA_H_
#define DMA_H_

#include "MKL25Z4.h"
#include "config.h"

void init_dma(uint16_t * source, uint32_t count);
void start_dma(void);
void DMA0_IRQHandler(void);

#endif /* DMA_H_ */
