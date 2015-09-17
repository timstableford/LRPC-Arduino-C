#ifndef TEST

#include <ctime>
#include <iostream>
#include <string>

#include "TCPStreamConnector.h"
#include "StreamParser.h"
#include "RPC.h"
#include "Object.h"

RPC *rpc;

void rpcCallback(Object &obj) {
	std::cout << "rpc callback called" << std::endl;
	std::cout << "Data is: " << obj.strAt(1) << std::endl;
}

void typeHandlerCallback(uint8_t *buffer, uint16_t size) {
	rpc->typeHandlerCallback(buffer, size);
}

int main(int argc, char *argv[]) {
	uint8_t buffer[1024];
	if(argc > 1) {
		TCPStreamConnector::TCPSocketServer sock(3333);

		//while(true) {
			sock.accept();

			StreamParser::TypeHandler h;
			h.type = TYPE_FUNCTION_CALL;
			h.callback = typeHandlerCallback;

			StreamParser p(TCPStreamConnector::networkReader, buffer, 1024, &h, 1, &sock);

			RPC::RPCContainer rpcc;
			rpcc.functionID = 10;
			rpcc.callback = rpcCallback;

			RPC rpca(TCPStreamConnector::networkWriter, &rpcc, 1, &sock);
			rpc = &rpca;

			while(p.parse() >= 0);
		//}

		std::cout << "Socket closed" << std::endl;
	} else {
		TCPStreamConnector::TCPSocketClient sock("localhost", "3333");
		RPC rpca(TCPStreamConnector::networkWriter, NULL, 0, &sock);
		rpca.call(10, "s", "hello world");
	}
}

#endif

