#include "Object.h"

#include <stdlib.h>

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
INT_GETSET(int64_t, int64At, htonll, ntohll)
INT_GETSET(uint8_t, uint8At, SIMPLE_FUNC, SIMPLE_FUNC)
INT_GETSET(uint16_t, uint16At, htons, ntohs)
INT_GETSET(uint32_t, uint32At, htonl, ntohl)
INT_GETSET(uint64_t, uint64At, htonll, ntohll)

static Object::ObjectType typesArray[] = {
  { Object::T_STRING, 0x01 }, //string - number of bytes to use for strlen
  { Object::T_INT8, sizeof(int8_t) }, //int8_t
  { Object::T_UINT8, sizeof(uint8_t) }, //uint8_t
  { Object::T_INT16, sizeof(int16_t) }, //int16_t
  { Object::T_UINT16, sizeof(uint16_t) }, //uint16_t
  { Object::T_INT32, sizeof(int32_t) }, //int32_t
  { Object::T_UINT32, sizeof(uint32_t) }, //uint32_t
  { Object::T_INT64, sizeof(int64_t) }, //int64_t
  { Object::T_UINT64, sizeof(uint64_t) }, //uint64_t
  { Object::T_FLOAT, sizeof(float) } //float
};

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
		this->dataTable = &(buffer[this->numObjects + (stringCount * this->typeSize(T_STRING)) + 1]);
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

float Object::floatAt(uint8_t index) {
	float v = 0;
	
	uint8_t *p = (uint8_t *)pointerAt(index);
	
	if(objectTypeAt(index) != T_FLOAT) {
		return 0;
	}
	
	uint8_t size = typeSize(T_FLOAT);
	for(uint8_t i = 0; i < size; i++) {
		((uint8_t *)&v)[i] = p[i];
	}
	
	return v;
}

bool Object::floatAt(uint8_t index, float data) {
	if(objectTypeAt(index) != T_FLOAT) {
		return false;
	}
	
	uint8_t *p = (uint8_t *)pointerAt(index);
	for(uint8_t i = 0; i < typeSize(T_FLOAT); i++) {
		p[i] = ((uint8_t *)&data)[i];
	}
	
	return true;
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

uint16_t Object::writeTo(NetworkWriter writer, void *userdata) {
	if(this->numObjects == 0 || this->dataTable == NULL) {
		return 0;
	}
	
	uint16_t written = 0;

	written += writer(userdata, &(this->numObjects), 1) ? 1 : 0;
	
	uint16_t size = this->numObjects + this->strNum(this->numObjects);
	written += writer(userdata, this->indexTable, size);
	
	size = this->getDataSize();
	written += writer(userdata, this->dataTable, size);
	
	return written;
}

uint8_t Object::typeSize(uint8_t type) {
	for(uint8_t i = 0; i < NUM_TYPES; i++) {
		if(typesArray[i].type == type) {
			return typesArray[i].size;
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
