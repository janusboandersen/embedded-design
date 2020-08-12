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

// nlohmann/json
#include <json.hpp>

// for convenience
using json = nlohmann::json;

const std::string REQUIRED_JSONRPC_VER("2.0");

/* Regex to match the menu options
 * https://solarianprogrammer.com/2011/10/12/cpp-11-regex-tutorial/
 * https://www.informit.com/articles/article.aspx?p=2079020
 */

/* Match GETTEMP followed by anything else*/
std::regex GET_TEMP("GETTEMP(.*)");

/* Matching of SETTEMP is now a METHOD and a PARAM
 * Match e.g. SETTEMP 37.1, with values possible like 37, 37., 37.1, 37.11
 * But there can only be one decimal point ( [\\.]? )
 * And then anything else can follow .* */
//std::regex SET_TEMP("SETTEMP ([0-9]+[\\.]?[0-9]*).*");
std::regex SET_TEMP("SETTEMP(.*)");

// struct to hold the deserialized RPC call
typedef struct {
	std::string protocol_ver;
	std::string method;
	std::string id;
	std::string params;
} rpc_t;


void * listen_thread_function(void * value) {

	float set_point_unsafe;
	std::string rec;
	json j_req;
	bool valid_rpc;
	rpc_t rpc_call;

	/* Create server */
	std::cout << "Starting server" << std::endl;
	SocketServer server(PORT_NUM);

	/* Bind and listen for connections
	 * blocks until a connection received
	 * or something failed */
	server.listen();

	// Loop here until SIGHUP
	while ( !exit_now.load() ) {

		/* Init */
		valid_rpc = false;

		/* Attempt to receive a message from the client */
		rec = server.receive(BUF_SIZE);

		/* Attempt to de-serialize the json-rpc request */
		std::cout << rec << std::endl;
		try
		{
			j_req = json::parse(rec);
			valid_rpc = true;
		}
		catch (json::parse_error& e) {
			// output exception information
			std::cout << "message: " << e.what() << '\n'
				      << "exception id: " << e.id << '\n'
			          << "byte position of error: " << e.byte << std::endl;
			valid_rpc = false;
		}

		/* Continue validation
		 * Now check for malformed JSON-RPC call
		 * Must at least contain:
		 * {
		 *   "jsonrpc": "2.0",
		 *   "method": "somemethod",
		 *   "id": "someid"
		 * }
		 * */
		if (valid_rpc) {
			// Iterator location, can check against j_req.end() or deref'ed with *
			auto protocol_def = j_req.find("jsonrpc");

			// Boolean values t/f if keys are contained
			auto method_def = j_req.contains("method");
			auto id_def = j_req.contains("id");

			// validate "jsonrpc":"2.0"
			if (protocol_def != j_req.end() ) {
				// Check that protocol version is 2.0
				std::string version_txt = (*protocol_def).dump();
				valid_rpc = ( REQUIRED_JSONRPC_VER.compare(version_txt) );
			}

			// validate that method and id are defined too
			if ( !(method_def && id_def) ) {
				valid_rpc = false;
			}
		}

		// If all validation OK
		if (valid_rpc) {
			// Insert values into rpc struct (implicit cast to std::string)
			rpc_call.protocol_ver = j_req["jsonrpc"];
			rpc_call.method = j_req["method"];
			rpc_call.id = j_req["id"];

		} else {
			// Tell the client there is an error
			json invalid_resp;
			invalid_resp["jsonrpc"] = REQUIRED_JSONRPC_VER;
			invalid_resp["error"]["code"] = "1";
			invalid_resp["error"]["message"] = "Parse error, or malformed use of protocol.";
			invalid_resp["error"]["data"] = rec;	//return the sent RPC call string
			invalid_resp["id"] = "0";

			// Send error message
			server.send(invalid_resp.dump() + "\n");

			// Restart loop to wait for new message
			continue;
		}

		/* Create a regex match object to extract match groups */
		std::smatch s_match;

		/* GET TEMP */
		if ( std::regex_search(rpc_call.method, s_match, GET_TEMP) && s_match.size() > 0 ) {
			// Build message to send
			json j_resp;
			j_resp["jsonrpc"] = REQUIRED_JSONRPC_VER;
			j_resp["result"] = cur_temp;
			j_resp["id"] = rpc_call.id;

			// Send temperature result
			server.send(j_resp.dump() + "\n");
		}

		/* SET TEMP */
		else if ( std::regex_search(rpc_call.method, s_match, SET_TEMP) && s_match.size() > 0  ) {

			// If params included
			if ( j_req.contains("params") ) {
				/* try to extract the matched set point value
				 * It is still unsafe, not sanitized */
				set_point_unsafe = j_req["params"][0];

			} else {

				std::string errmsg = "Please include params object, like \"params\": [40.4]";
				json invalid_resp;
				invalid_resp["jsonrpc"] = REQUIRED_JSONRPC_VER;
				invalid_resp["error"]["code"] = "2A";
				invalid_resp["error"]["message"] = errmsg;
				invalid_resp["error"]["data"] = j_req;
				invalid_resp["id"] = rpc_call.id;

				// Send error message
				server.send(invalid_resp.dump() + "\n");

				// Go back and wait for next message
				continue;
			}

			// Only change if the desired temp is inside valid range
			if (set_point_unsafe >= TEMP_MIN && set_point_unsafe <= TEMP_MAX) {
				set_point = set_point_unsafe;
			} else {
				std::stringstream message;
				message << "Invalid temperature range, must be between "
						<< TEMP_MIN << " and " << TEMP_MAX << ".\n";

				json invalid_resp;
				invalid_resp["jsonrpc"] = REQUIRED_JSONRPC_VER;
				invalid_resp["error"]["code"] = "2B";
				invalid_resp["error"]["message"] = message.str();
				invalid_resp["error"]["data"] = j_req;
				invalid_resp["id"] = rpc_call.id;

				// Send error message
				server.send(invalid_resp.dump() + "\n");
			}

		} else {
			/* Valid RPC, but unrecognized method */
			json invalid_resp;
			invalid_resp["jsonrpc"] = REQUIRED_JSONRPC_VER;
			invalid_resp["error"]["code"] = "3";
			invalid_resp["error"]["message"] = "Method not recognized.";
			invalid_resp["error"]["data"] = "Called method: " + rpc_call.method;
			invalid_resp["id"] = rpc_call.id;

			// Send error message
			server.send(invalid_resp.dump() + "\n");

		}


	} //SIGHUP received

	pthread_exit(NULL);

}


