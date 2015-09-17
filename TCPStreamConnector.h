/*
 * TCPStreamConnector.h
 *
 *  Created on: 17 Sep 2015
 *      Author: tstableford
 */
#ifdef LINUX
#ifndef TCPSTREAMCONNECTOR_H_
#define TCPSTREAMCONNECTOR_H_

#include <boost/asio.hpp>
#include <stdint.h>
#include "NetworkUtil.h"

using boost::asio::ip::tcp;


namespace TCPStreamConnector {
	class TCPSocket {
	public:
		tcp::socket *getSocket();
	protected:
		tcp::socket *sock;
	};

	class TCPSocketServer : TCPSocket {
	public:
		TCPSocketServer(int port);
		~TCPSocketServer();
		void accept();
		tcp::socket *getSocket();
	private:
		tcp::acceptor *acceptor;
		boost::asio::io_service *io_service;
	};

	class TCPSocketClient : TCPSocket {
	public:
		TCPSocketClient(char *endpoint, char *port);
		~TCPSocketClient();
	private:
		boost::asio::io_service *io_service;
	};

	uint16_t networkWriter(void *userdata, uint8_t *data, uint16_t length);
	int16_t networkReader(void *userdata);
}




#endif /* TCPSTREAMCONNECTOR_H_ */
#endif
