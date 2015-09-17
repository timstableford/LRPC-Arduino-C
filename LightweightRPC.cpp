#ifndef TEST

#include <ctime>
#include <iostream>
#include <string>

#include "TCPStreamConnector.h"
#include "StreamParser.h"

int main(int argc, char *argv[]) {
	uint8_t buffer[1024];
	TCPStreamConnector::TCPSocketServer sock(3333);

	while(true) {
		sock.accept();
		StreamParser p(TCPStreamConnector::networkReader, buffer, 1024, NULL, 0, &sock);
		while(p.parse() >= 0);
	}

	std::cout << "Socket closed" << std::endl;
}

#endif

