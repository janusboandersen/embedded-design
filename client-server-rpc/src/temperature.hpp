/*
 * temperature.hpp
 *
 *  Created on: 29 Mar 2020
 *      Author: janus
 */

#ifndef TEMPERATURE_HPP_
#define TEMPERATURE_HPP_

/* ADC Channel 0 */
#define ADC 0

/* Valid temperature control range */
extern float TEMP_MIN;
extern float TEMP_MAX;

/* Interval between temperature reading (seconds) */
extern int TEMP_SAMPLE_INTERVAL;

/* These enable communication between threads */
extern float cur_temp;
extern float set_point;
extern std::atomic<bool> exit_now;

/* @brief Thread function to monitor temperature in the background
 * update the global variable CUR_TMP
 * */
void * temp_monitor_thread_function(void * value);

/* Convert ADC code to temperature in degrees celcius
 * @param adc_value ADC code (0 to 4095)
 * @return temperature in C */
float conv_temperature(int adc_value);

/* @brief Read a value from ADC channel
 * @param channel the ADC channel 0-7
 * @return 12-bit ADC code (0-4095) */
int read_analog(int channel);

#endif /* TEMPERATURE_HPP_ */
