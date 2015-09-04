#include "Object.h"

#define INT_GETSET(x, y, ct, cf) x Object::y(uint8_t index) { \
	x *pointer = (x *)this->pointerAt(index); \
	if(pointer == NULL) { \
		return 0; \
	} \
	\
	return cf(*pointer); \
} \
bool Object::y(uint8_t index, x data) { \
	x *pointer = (x *)this->pointerAt(index); \
	\
	if(pointer == NULL) { \
		return false; \
	} \
	\
	*pointer = ct(data); \
	\
	return true; \
}

#define SIMPLE_FUNC(x) x

INT_GETSET(int8_t, int8At, SIMPLE_FUNC, SIMPLE_FUNC)
INT_GETSET(int16_t, int16At, htons, ntohs)
INT_GETSET(int32_t, int32At, htonl, ntohl)
INT_GETSET(uint8_t, uint8At, SIMPLE_FUNC, SIMPLE_FUNC)
INT_GETSET(uint16_t, uint16At, htons, ntohs)
INT_GETSET(uint32_t, uint32At, htonl, ntohl)

Object::Object(uint8_t *indexTable, uint8_t numObjects, uint8_t *dataTable) {
	this->indexTable = indexTable;
	this->dataTable = dataTable;
	this->numObjects = numObjects;
}

Object::Object(uint8_t *indexTable, uint8_t numObjects) {
	this->indexTable = indexTable;
	this->dataTable = NULL;
	this->numObjects = numObjects;
}

Object::Object(uint8_t *buffer) {
	this->indexTable = &(buffer[1]);
	this->numObjects = buffer[0];
	if(numObjects > 0) {
		uint8_t stringCount = this->strNum(this->numObjects);
		this->dataTable = &(buffer[this->numObjects + (stringCount * this->typeSize(T_STRING))]);
	} else {
		this->dataTable = 0;
	}
}

Object::~Object() {
}

uint8_t Object::strNum(uint8_t index) {
	uint8_t count = 0;
	for(uint8_t i = 0; i < index; i++) {
		if(this->indexTable[i] == T_STRING) {
			count++;
		}
	}
	
	return count;
}

uint16_t Object::indexOf(uint8_t index) {
	uint16_t dataIndex = 0;
	
	if(index > this->numObjects) {
		return 0;
	}
	
	for(uint16_t i = 0; i < index; i++) {
		switch(this->indexTable[i]) {
			case T_STRING:
			{
				dataIndex += this->strlenAt(i);
				break;
			}
			default:
				dataIndex += this->typeSize(this->indexTable[i]);
				break;
		}
	}
	
	return dataIndex;
}

uint8_t Object::strlenAt(uint8_t index) {
	uint8_t sizeIndex = this->numObjects + this->strNum(index);
	return this->indexTable[sizeIndex];
}

char *Object::strAt(uint8_t index) {
	return (char *)pointerAt(index);
}

uint8_t Object::getNumObjects() {
	return this->numObjects;
}
	
uint8_t Object::objectTypeAt(uint8_t index) {
	if(this->numObjects <= 0 || index >= this->numObjects) {
		return T_NONE;
	}
	return this->indexTable[index];
}
	
bool Object::strAt(uint8_t index, char *str, uint16_t stringLen) {
	if(stringLen > this->strlenAt(index)) {
		return false;
	}
	
	char *storedStr = strAt(index);
	
	if(storedStr == NULL) {
		return false;
	}
	
	for(uint16_t i = 0; i < stringLen; i++) {
		storedStr[i] = str[i];
	}
	
	return true;
}

void Object::setDataBuffer(uint8_t *buffer) {
	this->dataTable = buffer;
}

uint16_t Object::getDataSize() {
	return this->indexOf(this->numObjects);
}

uint16_t Object::getSize() {
	return 1 + this->numObjects + (this->typeSize(T_STRING) * strNum(this->numObjects)) + this->getDataSize();
}

uint16_t Object::writeTo(NetworkWriter writer) {
	if(this->numObjects == 0 || this->dataTable == NULL) {
		return 0;
	}
	
	uint16_t written = 0;
	
	written += writer(&(this->numObjects), 1) ? 1 : 0;
	
	uint16_t size = this->numObjects + this->strNum(this->numObjects);
	written += writer(this->indexTable, size);
	
	size = this->getDataSize();
	written += writer(this->dataTable, size);
	
	return written;
}

uint8_t Object::typeSize(uint8_t type) {
	for(uint8_t i = 0; i < NUM_TYPES; i++) {
		if(typesArray[i][0] == type) {
			return typesArray[i][1];
		}
	}
	
	return 0;
}

void *Object::pointerAt(uint8_t index) {
	if(this->dataTable == NULL) {
		return NULL;
	}
	
	if(index > this->numObjects) {
		return NULL;
	}
	
	return (void *)(&(this->dataTable[this->indexOf(index)]));
}

const uint8_t Object::typesArray[][NUM_TYPES] = {
	{ 0x01, 0x01 }, //string
	{ 0x02, 0x01 }, //int8_t
	{ 0x03, 0x01 }, //uint8_t
	{ 0x04, 0x02 }, //int16_t
	{ 0x05, 0x02 }, //uint16_t
	{ 0x06, 0x04 }, //int32_t
	{ 0x07, 0x04 }, //uint32_t
	{ 0x0C, 0x04 } //float
};
