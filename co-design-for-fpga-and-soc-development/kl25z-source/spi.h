
#ifndef SPI_H_
#define SPI_H_

#include "MKL25Z4.h"

#define SPI1_SCLK_POS 2		// PTE2
#define SPI1_MOSI_POS 3		// PTE3
#define SPI1_MISO_POS 1		// PTE1
#define SPI1_PCS0_POS 4		// PTE4: SS

void Init_SPI1(void);

// Send a byte
uint8_t SPIsend(uint8_t d_out);

void Test_SPI_Loopback(void);

#endif /* SPI_H_ */
