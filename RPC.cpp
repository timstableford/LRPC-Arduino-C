#include "Object.h"
#include "RPC.h"
#include "StreamParser.h"
#include "NetworkUtil.h"

#include <stdarg.h>
#include <string.h>

#define EXTRA_TYPES 1

typedef struct {
	Object::TYPES type;
	union TypeData {
		char *str;
		uint8_t uint8;
		int8_t int8;
		uint16_t uint16;
		int16_t int16;
		uint32_t uint32;
		int32_t int32;
		uint64_t uint64;
		int64_t int64;
		float flt;
	} data;
} Argument;

void RPC::typeHandlerCallback(uint8_t *buffer, uint16_t size) {
	Object obj(buffer);
	if(obj.getNumObjects() > 0) {
		if(obj.objectTypeAt(0) == Object::T_UINT16) {
			for(uint16_t i = 0; i < this->numRPCs; i++) {
				if(this->rpcs[i].functionID == obj.uint16At(0)) {
					this->rpcs[i].callback(obj);
					break;
				}
			}
		}
	}
}

RPC::RPC(NetworkWriter writer, RPCContainer *rpcs, uint16_t numRPCs) {
	this->writer = writer;
	this->rpcs = rpcs;
	this->numRPCs = numRPCs;
}

RPC::~RPC() {
}

Object::TYPES RPC::getType(char c) {
	switch(c) {
		case 's':
			return Object::T_STRING;
		case 'c':
			return Object::T_INT8;
		case 'C':
			return Object::T_UINT8;
		case 'd':
			return Object::T_INT16;
		case 'D':
			return Object::T_UINT16;
		case 'l':
			return Object::T_INT32;
		case 'L':
			return Object::T_UINT32;
		case 'm':
			return Object::T_INT64;
		case 'M':
			return Object::T_UINT64;
		case 'f':
			return Object::T_FLOAT;
		default:
			return Object::T_NONE;
	}
}

uint16_t RPC::call(uint16_t functionID, const char *fmt, ...) {
	va_list argp;
	uint16_t fmtLen = strlen(fmt);
	
	uint16_t numArgs = fmtLen + EXTRA_TYPES;
	Argument args[numArgs];
	
	// Setup function ID
	args[0].type = Object::T_UINT16;
	args[0].data.uint16 = functionID;
	
	va_start(argp, fmt);
	
	// Temporarily store the arguments in a list
	for(uint16_t i = EXTRA_TYPES; i < numArgs; i++) {
		args[i].type = this->getType(fmt[i - EXTRA_TYPES]);
		switch(args[i].type) {
			case Object::T_STRING:
				args[i].data.str = va_arg(argp, char *);
				break;
			case Object::T_INT8:
				args[i].data.int8 = (int8_t)va_arg(argp, int);
				break;
			case Object::T_UINT8:
				args[i].data.uint8 = (uint8_t)va_arg(argp, int);
				break;
			case Object::T_INT16:
				args[i].data.int16 = (int16_t)va_arg(argp, int);
				break;
			case Object::T_UINT16:
				args[i].data.uint16 = (uint16_t)va_arg(argp, int);
				break;
			case Object::T_INT32:
				args[i].data.int32 = (int32_t)va_arg(argp, long);
				break;
			case Object::T_UINT32:
				args[i].data.uint32 = (uint32_t)va_arg(argp, long);
				break;
			case Object::T_INT64:
				args[i].data.int64 = (int64_t)va_arg(argp, long long);
				break;
			case Object::T_UINT64:
				args[i].data.uint64 = (uint64_t)va_arg(argp, long long);
				break;
			case Object::T_FLOAT:
				args[i].data.flt = (float)va_arg(argp, double);
				break;
			case Object::T_NONE:
				return 0;
		}
	}
	
	va_end(argp);
	
	// Calculate the length needed for the object buffers before including string lengths
	uint16_t length = 0, indexSize = 0, numStr = 0; //index size start as 1 for function call id
	for(uint8_t i = 0; i < numArgs; i++) {
		if(args[i].type == Object::T_STRING) {
			length += strlen(args[i].data.str) + 1;
			numStr++;
		} else {
			length += Object::typeSize(args[i].type);
		}
		indexSize++;
	}
	
	//1 byte to indicate type for each object, additional bytes to represent string length
	uint8_t indexTable[indexSize + numStr * Object::typeSize(Object::T_STRING)];
	//length of data buffer, including string length (+null terminators)
	uint8_t buffer[length];
	
	//Propogate the buffer index table with all the types and the string lengths
	uint16_t stringIndex = 0;
	for(uint8_t i = 0; i < numArgs; i++) {
		indexTable[i] = args[i].type;
		if(indexTable[i] == Object::T_STRING) {
			indexTable[indexSize + stringIndex] = strlen(args[i].data.str) + 1;
			stringIndex++;
		}
	}
	
	// Put the buffer and index table into and object
	Object o(indexTable, numArgs, buffer);
	
	// For each item in the object, assign it's data from the temporary buffer
	for(uint8_t i = 0; i < o.getNumObjects(); i++) {
		if(o.objectTypeAt(i) != args[i].type) {
			return 0;
		}
		switch(args[i].type) {
			case Object::T_STRING:
				o.strAt(i, args[i].data.str, (uint8_t)(strlen(args[i].data.str) + 1));
				break;
			case Object::T_INT8:
				o.int8At(i, args[i].data.int8);
				break;
			case Object::T_UINT8:
				o.uint8At(i, args[i].data.uint8);
				break;
			case Object::T_INT16:
				o.int16At(i, args[i].data.int16);
				break;
			case Object::T_UINT16:
				o.uint16At(i, args[i].data.uint16);
				break;
			case Object::T_INT32:
				o.int32At(i, args[i].data.int32);
				break;
			case Object::T_UINT32:
				o.uint32At(i, args[i].data.uint32);
				break;
			case Object::T_INT64:
				o.int64At(i, args[i].data.int64);
				break;
			case Object::T_UINT64:
				o.uint64At(i, args[i].data.uint64);
				break;
			case Object::T_FLOAT:
				o.floatAt(i, args[i].data.flt);
				break;
			case Object::T_NONE:
				return 0;
		}
	}
	
	StreamParser::PacketHeader ph = StreamParser::makePacket(TYPE_FUNCTION_CALL, o.getSize());
	
	uint16_t written = this->writer((uint8_t *)(&ph), sizeof(ph));
	if(written < sizeof(ph)) {
		return 0;
	}
	
	written += o.writeTo(this->writer);
	
	return written;
}
