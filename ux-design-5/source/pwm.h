
#ifndef PWM_H_
#define PWM_H_

#include "MKL25Z4.h"  //also includes the types we need...

#define PWM_PERIOD 1000
#define PWM_POS 2 //PTC2

int pwmInit(uint16_t period);

void init_dbg_pin(void);

#endif /* PWM_H_ */
