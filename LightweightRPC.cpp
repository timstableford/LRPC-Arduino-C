#if !defined(TEST) && defined(LINUX)

#include <ctime>
#include <iostream>
#include <string>

#include "TCPStreamConnector.h"
#include "StreamParser.h"
#include "RPC.h"
#include "Object.h"

void rpcCallback(void *userdata, Object &obj) {
	std::cout << "rpc callback called" << std::endl;
	std::cout << "Data is: " << obj.strAt(1) << std::endl;
}

void rpcHandlerCallback(void *userdata, uint8_t *buffer, uint16_t size) {
	((RPC *)userdata)->typeHandlerCallback(buffer, size);
}

int main(int argc, char *argv[]) {
	uint8_t buffer[1024];
	if(argc > 1) {
		TCPStreamConnector::TCPSocketServer sock(3333);

		sock.accept();

		RPC::RPCContainer rpcc;
		rpcc.functionID = 10;
		rpcc.callback = rpcCallback;

		RPC rpc(TCPStreamConnector::networkWriter, &rpcc, 1, &sock);

		StreamParser::TypeHandler h;
		h.type = TYPE_FUNCTION_CALL;
		h.callback = rpcHandlerCallback;
		h.userdata = &rpc;

		StreamParser p(TCPStreamConnector::networkReader, buffer, 1024, &h, 1, &sock);

		while(p.parse() >= 0);

		std::cout << "Socket closed" << std::endl;
	} else {
		TCPStreamConnector::TCPSocketClient sock((char *)"localhost", (char *)"3333");
		RPC rpc(TCPStreamConnector::networkWriter, NULL, 0, &sock);
		rpc.call(10, "s", "hello world");
	}
}

#endif

