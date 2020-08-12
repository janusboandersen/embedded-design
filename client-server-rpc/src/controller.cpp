/*
 * controller.cpp
 *
 *  Created on: 30 Mar 2020
 *      Author: janus
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <atomic>
#include <algorithm>
#include "controller.hpp"

using namespace std;

#define PWM_DUTYCYCLE_PATH "/sys/class/pwm/pwmchip0/pwm-0:0/duty_cycle"
#define PWM_PERIOD_PATH "/sys/class/pwm/pwmchip0/pwm-0:0/period"
#define PWM_ENABLE_PATH "/sys/class/pwm/pwmchip0/pwm-0:0/enable"

/* @brief Initializes the PWM
 * @param period (int) the initial period in us
 * @param duty_cycle (int) the initial duty cycle in us
 */
void enable_pwm(int period, int duty_cycle) {
   fstream fs;

   /* Write period */
   fs.open(PWM_PERIOD_PATH, fstream::out);
   fs << period;
   fs.close();

   /* Write duty cycle */
   fs.open(PWM_DUTYCYCLE_PATH, fstream::out);
   fs << duty_cycle;
   fs.close();

   /* Write enable */
   fs.open(PWM_ENABLE_PATH, fstream::out);
   fs << 1;
   fs.close();
}

/* @brief Shuts off the PWM
 */
void disable_pwm(void) {
   fstream fs;

   /* Write period */
   fs.open(PWM_PERIOD_PATH, fstream::out);
   fs << 0;
   fs.close();

   /* Write duty cycle */
   fs.open(PWM_DUTYCYCLE_PATH, fstream::out);
   fs << 0;
   fs.close();

   /* Write enable */
   fs.open(PWM_ENABLE_PATH, fstream::out);
   fs << 0;
   fs.close();
}


/* @brief Write duty cycle vaue to PWM
 * @param duty_cycle (int) duty cycle in us
 */
void write_pwm_duty(int duty_cycle){
   fstream fs;

   /* Open PWM fd and write a value */
   fs.open(PWM_DUTYCYCLE_PATH, fstream::out);
   fs << duty_cycle;
   fs.close();
}



void * controller_thread_function(void * value) {

	/* Initialize the PWM with zero duty cycle */
    enable_pwm(PWM_PERIOD, 0);

    /* difference between set point and current temp */
    float cur_err = 0;
    float prev_err = 0;
    float deriv_err = 0;
    int new_duty_cycle = 0;

    while( !exit_now.load() ) {

    	// Compute controller action
    	cur_err = set_point - cur_temp;
    	deriv_err = cur_err - prev_err;

    	// Compute PD
    	new_duty_cycle = new_duty_cycle + (int) (Kp * cur_err + Kd * deriv_err);

    	// Limit duty cycle between 0 to PWM_PERIOD
    	if ( new_duty_cycle > PWM_PERIOD) {
    		new_duty_cycle = PWM_PERIOD;
    	} else if (new_duty_cycle < 0) {
    		new_duty_cycle = 0;
    	}

    	//output for debugging
    	//std::cout << "New duty cycle: " << new_duty_cycle << std::endl;

    	// Set the new duty cycle
    	write_pwm_duty(new_duty_cycle);

    	// update for next round
    	prev_err = cur_err;

    	// wait until time to update again
    	sleep(controller_interval);
    }

    /* Ending - power down PWM*/
    disable_pwm();
	pthread_exit(NULL);

}
