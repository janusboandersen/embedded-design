
#ifndef PWM_H_
#define PWM_H_

#include "MKL25Z4.h"  //also includes the types we need...

#define BLUE_LED_POS (1) 									// PTD1
#define DBG_PIN (1)											// Use PTB1 for measuring time
#define TOGGLE_BLUE_LED() FPTD->PTOR = 1UL << BLUE_LED_POS	// Macro to Fast toggle PTB1
#define TOGGLE_DEBUG_PIN() FPTB->PTOR = 1UL << DBG_PIN		// Macro to Fast toggle PTB1

#define PWM_PERIOD 1000
#define YELLOW_LED_POS 2 //PTC2

int pwmInit(uint16_t period);

void init_dbg_pin(void);

#endif /* PWM_H_ */
