#if defined(LINUX)

#include <ctime>
#include <iostream>
#include <boost/thread.hpp>
#include <string>
#include <unistd.h>

#include "TCPStreamConnector.h"
#include "StreamParser.h"
#include "RPC.h"
#include "Object.h"

#define BUFFER_SIZE 1024
#define PORT_NUMBER 3333
#define STR(x) #x

namespace {
void rpcData(RPC &rpc, Object &obj, void *userdata) {
  std::cout << "(S) Data from client is: " << obj.strAt(1) << std::endl << std::flush;
  rpc.call(2, "s", "Quitting");
  std::cout << "(S) Sent response" << std::endl << std::flush;
}

void rpcQuit(RPC &rpc, Object &obj, void *userdata) {
  std::cout << "(C) Quit called from server\n" << std::endl << std::flush;
  std::cout << "Data is " << obj.strAt(1) << std::endl << std::flush;
  *((bool *)userdata) = false;
}

RPC::RPCContainer rpcs[] = {
  { 1, rpcData, NULL },
  { 2, rpcQuit, NULL }
};

void serverFunc() {
	uint8_t buffer[BUFFER_SIZE];
	TCPStreamConnector::TCPSocketServer sock(PORT_NUMBER);

	std::cout << "(S) Listening for client..." << std::endl << std::flush;
	sock.accept();
	std::cout << "(S) Client connected" << std::endl << std::flush;

	RPC rpc(TCPStreamConnector::networkWriter, rpcs, sizeof(rpcs) / sizeof(RPC::RPCContainer), &sock);

	StreamParser p(TCPStreamConnector::networkReader, buffer, sizeof(buffer), rpc.getHandler(), 1, &sock);

	while(p.parse() >= 0);

	std::cout << "(S) Server socket closed" << std::endl << std::flush;
}

void clientFunc() {
	bool run = true;

  std::cout << "(C) Connecting to server" << std::endl;
	TCPStreamConnector::TCPSocketClient sock((char *)"localhost", (char *)std::to_string(PORT_NUMBER).c_str());
  std::cout << "(C) Connected to server" << std::endl;

  rpcs[1].userdata = &run;
	RPC rpc(TCPStreamConnector::networkWriter, rpcs, sizeof(rpcs) / sizeof(RPC::RPCContainer), &sock);

	uint8_t buffer[BUFFER_SIZE];
	StreamParser p(TCPStreamConnector::networkReader, buffer, sizeof(buffer), rpc.getHandler(), 1, &sock);

	std::cout << "(C) Calling hello world to server" << std::endl << std::flush;
	rpc.call(1, "s", "hello world");

	std::cout << "(C) Waiting for response from server" << std::endl << std::flush;
	while(p.parse() >= 0 && run);
}
} // namespace

void socketTest() {
	boost::thread serverThread(serverFunc);
	usleep(1000000);
	boost::thread clientThread(clientFunc);

  std::cout << "(M) Awaiting client thread" << std::endl << std::flush;
	clientThread.join();
	std::cout << "(M) Client thread ended" << std::endl << std::flush;
	serverThread.join();
	std::cout << "(M) Server thread ended" << std::endl << std::flush;
}

#endif

