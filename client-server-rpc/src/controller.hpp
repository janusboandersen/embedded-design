/*
 * controller.hpp
 *
 *  Created on: 30 Mar 2020
 *      Author: janus
 */

#ifndef CONTROLLER_HPP_
#define CONTROLLER_HPP_

// For PWM we always use this period
#define PWM_PERIOD 4000

// For controller, we use these coefficients
extern float Kp;
extern float Kd;

extern float cur_temp;
extern float set_point;
extern std::atomic<bool> exit_now;
extern int controller_interval;

/* @brief Shuts off the PWM
 */
void disable_pwm(void);


void enable_pwm(int period, int duty_cycle);


void write_pwm_duty(int duty_cycle);

void * controller_thread_function(void * value);

#endif /* CONTROLLER_HPP_ */
