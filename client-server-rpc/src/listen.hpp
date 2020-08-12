/*
 * listen.hpp
 *
 *  Created on: 30 Mar 2020
 *      Author: janus
 */

#ifndef LISTEN_HPP_
#define LISTEN_HPP_

/* For socket server communication */
#define PORT_NUM 54324
#define BUF_SIZE 256

extern float TEMP_MIN;
extern float TEMP_MAX;

extern float cur_temp;
extern float set_point;
extern std::atomic<bool> exit_now;

void * listen_thread_function(void * value);


#endif /* LISTEN_HPP_ */
