/*
 * TCPStreamConnector.cpp
 *
 *  Created on: 17 Sep 2015
 *      Author: tstableford
 */
#ifdef LINUX
#include "TCPStreamConnector.h"
#include "StreamParser.h"

#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <stdint.h>

namespace TCPStreamConnector {
using boost::asio::ip::tcp;

TCPSocketServer::TCPSocketServer(int port) {
	try {
		this->io_service = new boost::asio::io_service();

		this->acceptor = new tcp::acceptor(*io_service, tcp::endpoint(tcp::v4(), port));

		this->sock = new tcp::socket(*io_service);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}

TCPSocketServer::~TCPSocketServer() {
	delete(this->acceptor);
	delete(this->sock);
	delete(this->io_service);
}

void TCPSocketServer::accept() {
	std::cout << "Listening for connection on port" << std::endl;
	if(this->sock != NULL) {
		delete(this->sock);
		this->sock = new tcp::socket(*io_service);
	}
	this->acceptor->accept(*sock);
	std::cout << "Client connected" << std::endl;
}

tcp::socket *TCPSocket::getSocket() {
	return this->sock;
}

TCPSocketClient::TCPSocketClient(char *endpoint, char *port) {
	try {
		this->io_service = new boost::asio::io_service();

		tcp::resolver resolver(*(this->io_service));
		tcp::resolver::query query(endpoint, port);
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

		this->sock = new tcp::socket(*(this->io_service));
		boost::asio::connect(*(this->sock), endpoint_iterator);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}

TCPSocketClient::~TCPSocketClient() {
	delete(this->sock);
	delete(this->io_service);
}

uint16_t networkWriter(void *userdata, uint8_t *data, uint16_t length) {
	if(userdata == NULL) {
		return 0;
	}
	TCPSocket *tcps = (TCPSocket *)userdata;
	std::vector<uint8_t> buffer(data, data + length);
	boost::system::error_code ignored_error;
	ssize_t result = boost::asio::write(*(tcps->getSocket()), boost::asio::buffer(buffer), ignored_error);
	if(result < 0) {
		result = 0;
	}
	return (uint16_t)result;
}

int16_t networkReader(void *userdata) {
	if(userdata == NULL) {
		return 0;
	}
	TCPSocket *tcps = (TCPSocket *)userdata;

	uint8_t data[1];
	ssize_t result = 0;
	try {
		result = boost::asio::read(*(tcps->getSocket()), boost::asio::buffer(data, 1));
	} catch (std::exception& e) {
		return -2;
	}
	if(result <= 0) {
		return -1;
	} else {
		return (uint16_t)data[0];
	}
}

}

#endif

