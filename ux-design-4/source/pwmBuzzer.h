
#ifndef PWMBUZZER_H_
#define PWMBUZZER_H_

#include "MKL25Z4.h"   // also includes the types we need...
#include "tonetable.h" // has the tone frequencies

#define BUZZER_POS 13 //PTA13 -> FTM1_CH1

extern volatile uint16_t totSeqRuns;
extern volatile uint16_t remSeqRuns;
extern volatile uint16_t * buzzSeq;		// holds the sequence of tones
extern volatile uint16_t buzzDelay;	// miliseconds tone length


int buzzerInit(void);

void buzzStop(void);
void buzzTone(uint16_t freq);


#endif /* PWMBUZZER_H_ */
