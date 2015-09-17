#if defined(TEST) && defined(LINUX)

#include <ctime>
#include <iostream>
#include <boost/thread.hpp>
#include <string>
#include <unistd.h>

#include "TCPStreamConnector.h"
#include "StreamParser.h"
#include "RPC.h"
#include "Object.h"

void rpcCallback(void *userdata, Object &obj) {
	switch(obj.uint16At(0)) {
	case 1:
		std::cout << "(S) Data from client is: " << obj.strAt(1) << std::endl << std::flush;
		((RPC *)userdata)->call(2, "s", "Quitting");
		break;
	case 2:
		std::cout << "(C) Quit called from server\n" << std::endl << std::flush;
		*((bool *)userdata) = false;
		break;
	}
}

void rpcHandlerCallback(void *userdata, uint8_t *buffer, uint16_t size) {
	((RPC *)userdata)->typeHandlerCallback(buffer, size);
}

void serverFunc() {
	uint8_t buffer[1024];
	TCPStreamConnector::TCPSocketServer sock(3333);

	std::cout << "(S) Listening for client..." << std::endl << std::flush;
	sock.accept();
	std::cout << "(S) Client connected" << std::endl << std::flush;

	RPC::RPCContainer rpcc;
	rpcc.functionID = 1;
	rpcc.callback = rpcCallback;

	RPC rpc(TCPStreamConnector::networkWriter, &rpcc, 1, &sock);
	rpcc.userdata = &rpc;

	StreamParser::TypeHandler h;
	h.type = TYPE_FUNCTION_CALL;
	h.callback = rpcHandlerCallback;
	h.userdata = &rpc;

	StreamParser p(TCPStreamConnector::networkReader, buffer, 1024, &h, 1, &sock);

	while(p.parse() >= 0);

	std::cout << "(S) Server socket closed" << std::endl << std::flush;
}

void clientFunc() {
	bool run = true;

	TCPStreamConnector::TCPSocketClient sock((char *)"localhost", (char *)"3333");

	RPC::RPCContainer rpcc;
	rpcc.functionID = 2;
	rpcc.callback = rpcCallback;
	rpcc.userdata = &run;
	RPC rpc(TCPStreamConnector::networkWriter, &rpcc, 1, &sock);

	StreamParser::TypeHandler h;
	h.type = TYPE_FUNCTION_CALL;
	h.callback = rpcHandlerCallback;
	h.userdata = &rpc;

	uint8_t buffer[1024];
	StreamParser p(TCPStreamConnector::networkReader, buffer, 1024, &h, 1, &sock);

	std::cout << "(C) Calling hello world to server" << std::endl << std::flush;
	rpc.call(1, "s", "hello world");

	std::cout << "(C) Waiting for response from server" << std::endl << std::flush;
	while(p.parse() >= 0 && run);
}

void socketTest() {
	boost::thread serverThread(serverFunc);
	usleep(1000000);
	boost::thread clientThread(clientFunc);

	clientThread.join();
	std::cout << "(M) Client thread ended" << std::endl << std::flush;
	serverThread.join();
	std::cout << "(M) Server thread ended" << std::endl << std::flush;
}

#endif

