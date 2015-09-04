#include "StreamParser.h"
#include "Object.h"
#include "RPC.h"

#include <stdlib.h>
#include <string.h>

uint8_t *testBuffer;
uint16_t testBufferSize;
uint16_t testBufferIndex;

uint8_t testCallBuffer[] = { 0x0, 0x10, 0x0, 0x18, 0x9a, 0x54, 0x4, 0x5, 0x5, 0x4, 0x1, 0x0, 0x1, 0x5a, 0xc0, 0x1, 0x90, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64, 0x0 };
uint16_t testCallIndex = 0;

void functionCallback(uint8_t *buffer, uint16_t size) {
	Object o(buffer);
	printf("Function callback called\n");
}

StreamParser::TypeHandler handlers[] = {
	{
		16, functionCallback
	}
};

int16_t streamReader() {
	if(testCallIndex < sizeof(testCallBuffer)) {
		return (int16_t)(testCallBuffer[testCallIndex++]);
	} else {
		return -1;
	}
}

bool writer(uint8_t *data, uint16_t size) {
	for(uint16_t i = 0; i < size; i++) {
		printf("0x%x, ", data[i]);
	}
	return true;
}

void printHex(void *buffer, int length) {
	for(int i = 0; i < length; i++) {
		printf("0x%x, ", ((uint8_t *)(buffer))[i]);
	}
}

int main(int argc, char *argv[]) {
	uint8_t buffer[256];
	uint16_t bufferSize = 256;
	StreamParser p(streamReader, buffer, bufferSize, handlers, 1);
	
	int16_t theInt = 400;
	uint16_t functionID = 1;
	
	char *stri = (char *)"hello world";
	
	uint8_t indexTable[] = { Object::T_UINT16, Object::T_INT16, Object::T_STRING, (uint8_t)(strlen(stri) + 1) };
	Object o(indexTable, 3);
	
	uint8_t dataBuffer[o.getDataSize()];
	o.setDataBuffer(dataBuffer);
	
	o.uint16At(0, functionID); //function id
	o.int16At(1, theInt); //payload argument
	o.strAt(2, stri, strlen(stri) + 1);
	
	if(o.int16At(1) != theInt) {
		printf("Retrieved int does not equal actual A:%d R:%d\n", theInt, o.int16At(1));
	}
	
	StreamParser::PacketHeader ph = StreamParser::makePacket(16, o.getSize());
	
	printHex(&ph, sizeof(ph));
	o.writeTo(writer);
	printf("\n");
	
	
	testBuffer = ((uint8_t *)(&ph));
	testBufferSize = sizeof(StreamParser::PacketHeader);
	testBufferIndex = 0;
	
	while(p.parse() >= 0);
	
	RPC rpc(writer);
	rpc.call(10, "cCds", -10, 10, 320, "hello world");
	printf("\n");
}
