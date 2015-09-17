#if defined(LINUX) && defined(TEST)

#include "StreamParser.h"
#include "Object.h"
#include "RPC.h"

#include <stdlib.h>
#include <string.h>

uint8_t *testBuffer;
uint16_t testBufferSize;
uint16_t testBufferIndex;

uint8_t testCallBuffer[] = { 0x0, 0x8, 0x0, 0x19, 0x79, 0xae, 0x5, 0x5, 0x2, 0x3, 0x4, 0x1, 0xc, 0x0, 0xa, 0xf6, 0xa, 0x1, 0x40, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64, 0x0 };
uint16_t testCallIndex = 0;

void printHex(void *buffer, int length) {
	for(int i = 0; i < length; i++) {
		printf("0x%x, ", ((uint8_t *)(buffer))[i]);
	}
}

uint16_t writer(void *userdata, uint8_t *data, uint16_t size) {
	for(uint16_t i = 0; i < size; i++) {
		printf("0x%x, ", data[i]);
	}
	return size;
}

void helloWorld(Object &obj) {
	printf("hello world function called %d, %d, %d, %s\n", obj.int8At(1), obj.uint8At(2), obj.int16At(3), obj.strAt(4));
}

RPC::RPCContainer rpcs[] = {
	{
		10, helloWorld
	}
};

RPC rpc(writer, rpcs, 1);

void functionCallback(uint8_t *buffer, uint16_t size) {
	rpc.typeHandlerCallback(buffer, size);
}

StreamParser::TypeHandler handlers[] = {
	{
		TYPE_FUNCTION_CALL, functionCallback
	}
};

int16_t streamReader(void *userdata) {
	if(testCallIndex < sizeof(testCallBuffer)) {
		return (int16_t)(testCallBuffer[testCallIndex++]);
	} else {
		return -1;
	}
}

int main(int argc, char *argv[]) {
	uint8_t buffer[256];
	uint16_t bufferSize = 256;
	StreamParser p(streamReader, buffer, bufferSize, handlers, 1, NULL);
	
	while(p.parse() >= 0);
	
	int16_t theInt = 400;
	float theFloat = 213423.23466432;
	uint16_t functionID = 1;
	
	char *stri = (char *)"hello world";
	
	uint8_t indexTable[] = { Object::T_UINT16, Object::T_INT16, Object::T_STRING, Object::T_FLOAT, Object::T_INT32, Object::T_INT64, (uint8_t)(strlen(stri) + 1) };
	Object o(indexTable, 6);
	
	uint8_t dataBuffer[o.getDataSize()];
	o.setDataBuffer(dataBuffer);
	
	o.uint16At(0, functionID); //function id
	o.int16At(1, theInt); //payload argument
	o.strAt(2, stri, strlen(stri) + 1);
	if(!o.floatAt(3, theFloat)) {
		printf("Failed to set float value\n");
	}
	o.int32At(4, 345589619);
	o.int64At(5, 9384760934765065ll);
	
	if(o.int64At(5) != 9384760934765065ll) {
		printf("Retrieved int64 does not equal actual A:%lld, R:%lld\n", (long long int)9384760934765065ll, (long long int)o.int64At(5));
	}
	
	if(o.int32At(4) != 345589619) {
		printf("Retrieved int32 does not equal actual\n");
	}
	
	if(o.floatAt(3) != theFloat) {
		printf("Retrieved float does not equal actual A:%f, R:%f\n", theFloat, o.floatAt(3));
	}
	
	if(o.int16At(1) != theInt) {
		printf("Retrieved int does not equal actual A:%d R:%d\n", theInt, o.int16At(1));
	}
	
	StreamParser::PacketHeader ph = StreamParser::makePacket(16, o.getSize());
	
	printf("generated packet: ");
	printHex(&ph, sizeof(ph));
	o.writeTo(writer);
	printf("\n");
	
	
	testBuffer = ((uint8_t *)(&ph));
	testBufferSize = sizeof(StreamParser::PacketHeader);
	testBufferIndex = 0;
	
	printf("rpc call: ");
	if(rpc.call(10, "cCds", -10, 10, 320, "hello world") <= 0) {
		printf("error doing rpc call");
	}
	printf("\n");
	
}

#endif
