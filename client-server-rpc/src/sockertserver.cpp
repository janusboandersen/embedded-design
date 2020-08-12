/*
 * sockertserver.cpp
 *
 *  Created on: 29 Mar 2020
 *      Author: janus
 */

#include "socketserver.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>	/* For bzero */
#include <stdexcept>
using namespace std;


SocketServer::SocketServer(int portNumber) {
	this->socketfd = -1;
	this->clientSocketfd = -1;
	this->portNumber = portNumber;
	this->clientConnected = false;
}

/* listen creates the socket and
 * blocks until an incoming connection established,
 * and then accepts the incoming connection */
int SocketServer::listen(){

	/* Make INET (TCP) socket */
	this->socketfd = socket(AF_INET, SOCK_STREAM, 0);

	/* Check that socket created OK */
    if (this->socketfd < 0){
    	perror("Socket Server: error opening socket.\n");
    	return 1;
    }

    /* Ensure all zeros in unused part of serverAddress
     * this is an alternative to memset */
    bzero((char *) &serverAddress, sizeof(serverAddress));

    /* Create struct for address */
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(this->portNumber);

    /* Attempt to bind to the socket address */
    if (bind(socketfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
    	perror("Socket Server: error on binding the socket.\n");
    	return 1;
    }

    /* system call listen */
    ::listen(this->socketfd, 5);

    /* Update address of client */
    socklen_t clientLength = sizeof(this->clientAddress);

    /* Accept connection */
    this->clientSocketfd = accept(this->socketfd,
    							  (struct sockaddr *) &this->clientAddress,
								  &clientLength);

    /* Check that client socket is OK */
    if (this->clientSocketfd < 0){
    	perror("Socket Server: Failed to bind the client socket properly.\n");
    	return 1;
    }
    return 0;
}

int SocketServer::send(std::string message){
	const char *writeBuffer = message.data();
	int length = message.length();
    int n = write(this->clientSocketfd, writeBuffer, length);
    if (n < 0){
       perror("Socket Server: error writing to server socket.");
       return 1;
    }
    return 0;
}

string SocketServer::receive(int size=1024){
    char readBuffer[size];
    memset(readBuffer, '\0', size);
    int n = read(this->clientSocketfd, readBuffer, sizeof(readBuffer));
    if (n < 0){
       perror("Socket Server: error reading from server socket.");
       throw "Socket probably closed.";
    }
    return string(readBuffer);
}

void SocketServer::closeClient() {
	close(this->clientSocketfd);
	this->clientConnected = false;
	this->clientSocketfd = -1;
}

SocketServer::~SocketServer() {
	close(this->socketfd);
	close(this->clientSocketfd);
}

