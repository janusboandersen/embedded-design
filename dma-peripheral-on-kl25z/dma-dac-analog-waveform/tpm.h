/*
 * @file	tpm.h
 * @author	Janus Bo Andersen
 * @date	Feb 2020
 * @brief	Interface for TPM init and IRQHandler for ex.2 wk5/6
 *
 */

#ifndef TPM_H_
#define TPM_H_

#include "MKL25Z4.h"
#include "config.h"

void init_dbg_pin(void);
void init_tpm_w_irq(uint32_t period_us);
void init_tpm_wo_irq(uint32_t period_us);
void start_tpm();
void stop_tpm();
void TPM0_IRQHandler();

#endif /* TPM_H_ */
