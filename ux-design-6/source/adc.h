
#ifndef ADC_H_
#define ADC_H_

#include "MKL25Z4.h"


// function for setting up the ADC
// The ADC is initalized in 16 bit mode
int Init_ADC(void);

// function for reading the ADC, in single ended mode
// this function is blocking, and returns when a conversion is finished
uint16_t Read_ADC(void);


#endif /* ADC_H_ */
