/*
 * @file	dac.h
 * @author	Janus Bo Andersen
 * @date	Feb 2020
 * @brief	Interface for init and functions for DAC for ex.2 week 5/6
 *
 */

#ifndef DAC_H_
#define DAC_H_

#include "MKL25Z4.h"
#include "config.h"

void init_dac(void);
void init_triangle_data();

#endif /* DAC_H_ */
