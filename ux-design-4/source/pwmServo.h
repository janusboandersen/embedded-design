
#ifndef PWMSERVO_H_
#define PWMSERVO_H_

#include "MKL25Z4.h"  //also includes the types we need...

/* Must create 50 Hz for the servo
 * 48 MHz / 16 (prescaler) / 60000 = 50 Hz
 * 1% duty cycle -> CnV = 600
 * 2% duty cycle -> CnV = 1200
 * 10% duty cycle -> CnV = 6000
 * etc...
 */
#define SERVO_PERIOD 60000
#define SERVO_POS 1 //PTA1

int servoInit(uint16_t period);


#endif /* PWMSERVO_H_ */
