/*
 * socketserver.hpp
 *
 *  Created on: 29 Mar 2020
 *      Author: janus
 */

#ifndef SOCKETSERVER_HPP_
#define SOCKETSERVER_HPP_

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string>

/**
 * @class SocketServer
 * @brief A class that encapsulates a server socket for network communication
 */
class SocketServer {
private:
	int 		portNumber;
    int 	    socketfd, clientSocketfd;
    struct 	    sockaddr_in   serverAddress;
    struct 	    sockaddr_in   clientAddress;
    bool		clientConnected;

public:
	SocketServer(int portNumber);
	virtual int listen();
	virtual int send(std::string message);
	virtual std::string receive(int size);
	virtual void closeClient();

	virtual ~SocketServer();
};


#endif /* SOCKETSERVER_HPP_ */
