
#ifndef SWITCH_H_
#define SWITCH_H_

#include "MKL25Z4.h"

#define MASK(x)	(1UL << (x))

#define SW_POS 1 // PTC1

// Reads the input registry for portc and masks with the switch pos
// The button is active low, thus the negation
// Resolves to 1 if switch is pressed!
#define SWITCH ( ~(PTC->PDIR) & MASK(SW_POS) )

void init_sw(void);

#endif /* SWITCH_H_ */
