/*
 * vib.h
 *
 *  Created on: 16 Mar 2020
 *      Author: janusboandersen
 */

#ifndef VIB_H_
#define VIB_H_

#include "MKL25Z4.h"

/* Set up pins for vibration */
#define VIB_POS 0 /* PTB0 */

#define VIB_ON		GPIOB->PCOR = (1UL << VIB_POS) // Active low
#define VIB_OFF		GPIOB->PSOR = (1UL << VIB_POS) // Inactive high

void vib_init(void);

#endif /* VIB_H_ */
