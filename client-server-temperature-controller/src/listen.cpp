	/*
 * listen.cpp
 *
 *  Created on: 30 Mar 2020
 *      Author: janus
 */
#include <iostream>
#include <string>
#include <sstream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <regex>
#include <stdexcept>
#include <atomic>
#include "socketserver.hpp"
#include "listen.hpp"


void * listen_thread_function(void * value) {

	/* Regex to match the menu options
	 * https://solarianprogrammer.com/2011/10/12/cpp-11-regex-tutorial/
	 * https://www.informit.com/articles/article.aspx?p=2079020
	 */

	/* Match GET TEMP followed by anything else*/
	std::regex GET_TEMP("GET TEMP(.*)");

	/* Match e.g. SET TEMP 37.1, with values possible like 37, 37., 37.1, 37.11
	 * But there can only be one decimal point ( [\\.]? )
	 * And then anything else can follow .* */
	std::regex SET_TEMP("SET TEMP ([0-9]+[\\.]?[0-9]*).*");
	float set_point_unsafe;

	/* Create server */
	std::cout << "Starting server" << std::endl;
	SocketServer server(PORT_NUM);

	/* Bind and listen for connections
	 * blocks until a connection received
	 * or something failed */
	server.listen();

	// Loop here until SIGHUP
	while ( !exit_now.load() ) {

		/* Attempt to receive a message from the client */
		std::string rec;
		rec = server.receive(BUF_SIZE);

		/* Create a regex match object to extract match groups */
		std::smatch s_match;

		/* GET TEMP */
		if ( std::regex_search(rec, s_match, GET_TEMP) && s_match.size() > 0 ) {
			std::stringstream message;
			message << "Current temperature is " << cur_temp << " deg. C.\n";
			server.send(message.str());
		}

		/* SET TEMP */
		else if ( std::regex_search(rec, s_match, SET_TEMP) && s_match.size() > 0  ) {
			/* extract the matched set point value
			 * It is still unsafe, not sanitized */
			set_point_unsafe = std::stof( s_match.str(1) );

			// Only change if the desired temp is inside valid range
			if (set_point_unsafe >= TEMP_MIN && set_point_unsafe <= TEMP_MAX) {
				set_point = set_point_unsafe;
			} else {
				std::stringstream message;
				message << "Invalid temperature range, must be between "
						<< TEMP_MIN << " and " << TEMP_MAX << ".\n";
				server.send(message.str());
			}
		}

		/* UNRECOGNIZED COMMAND */
		else {
			std::stringstream message;
			message << "Invalid command. Use either \"GET TEMP\" or \"SET TEMP <float value>\"" << ".\n";
			server.send(message.str());
		}

	} //SIGHUP received

	pthread_exit(NULL);

}


