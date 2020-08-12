/*
 * temperature.cpp
 *
 *  Created on: 29 Mar 2020
 *      Author: janus
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <atomic>
#include "temperature.hpp"

using namespace std;

/* We need to get the value from in_voltage0_raw for channel 0 */
#define ADC_PATH "/sys/bus/iio/devices/iio:device0/in_voltage"
#define SUFFIX "_raw"

/* @brief Thread function to monitor temperature in the background
 * update the global variable CUR_TMP
 * */
void * temp_monitor_thread_function(void * value) {

	int adc_value;
	float temp;

	while( !exit_now.load() ) {

		// Take a sample and convert to temperature
		adc_value = read_analog(ADC);
		temp = conv_temperature(adc_value);

		// Update the global variable
		cur_temp = temp;

		// Sleep until next sample - this is also a thread cancellation point
		sleep(TEMP_SAMPLE_INTERVAL);
	}

	pthread_exit(NULL);
}


/* @brief Convert ADC code to temperature in degrees celcius
 * @param adc_value ADC code (0 to 4095)
 * @return temperature in C */
float conv_temperature(int adc_value) {
	/* ADC resolution 12-bits (4096), max-value 1.8 V */
	float cur_voltage = adc_value * (1.80f / 4096.0f);

	/* Convert from voltage reading to celsius */
	return (cur_voltage * 100.0f);
}

/* @brief Read a value from ADC channel
 * @param channel the ADC channel 0-7
 * @return 12-bit ADC code (0-4095) */
int read_analog(int channel){
   stringstream ss;
   fstream fs;
   int adc_value;

   /* Make path for the ADC channel fd */
   ss << ADC_PATH << channel << SUFFIX;

   /* Open ADC fd and read a value */
   fs.open(ss.str().c_str(), fstream::in);
   fs >> adc_value;
   fs.close();
   return adc_value;
}
