/*
 * test.cpp
 *
 *  Created on: 8 Nov 2019
 *      Author: janus
 */

#include <iostream>
#include <cstdio>
#include <string>
#include <sstream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <regex>
#include <stdexcept>
#include <atomic>
#include <csignal>
#include "temperature.hpp"
#include "socketserver.hpp"
#include "listen.hpp"
#include "controller.hpp"



/* Valid temperature control range */
float TEMP_MIN = 20.0;
float TEMP_MAX = 60.0;

/* Interval between temperature reading (seconds) */
int TEMP_SAMPLE_INTERVAL = 2; //sec

/* Update controller every ... */
int controller_interval = 1; //sec

// For controller, we use these coefficients
float Kp = 50;
float Kd = 0;

/* These globals are for communication between threads */
float cur_temp = 0;
float set_point = 0;

/* Coordinate clean exit :) */
std::atomic<bool> exit_now(false);

void sig_handler(int signo) {

    // React to caught signal
    if (signo == SIGHUP) {
        std::cout << "Received SIGHUP. Bye!" << std::endl;

        // Tell all threads to end now, cleanly
        exit_now.store(true);
    }
}

int main(void) {

	/* Set up signal handler */
    if ( signal(SIGHUP, sig_handler) == SIG_ERR ) {
		std::cout << "Can't register SIGINT" << std::endl;
	}

	pthread_t listen_thread;
	if ( pthread_create(&listen_thread, NULL, &listen_thread_function, NULL) ) {
			std::cout << "Could not create thread" << std::endl;
			return EXIT_FAILURE;
	}

	pthread_t temp_thread;
	if ( pthread_create(&temp_thread, NULL, &temp_monitor_thread_function, NULL) ) {
			std::cout << "Could not create thread" << std::endl;
			return EXIT_FAILURE;
	}

	pthread_t control_thread;
	if ( pthread_create(&control_thread, NULL, &controller_thread_function, NULL) ) {
			std::cout << "Could not create thread" << std::endl;
			return EXIT_FAILURE;
	}

	/* Temperature thread has been stopped by SIGHUP */
	pthread_join(temp_thread, NULL);

	/* Controller thread has been stopped by SIGHUP */
	pthread_join(control_thread, NULL);

	/* Safe to kill the listener now */
	pthread_cancel(listen_thread);
	pthread_join(listen_thread, NULL);

	return 0;
}
