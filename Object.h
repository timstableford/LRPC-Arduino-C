#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "NetworkUtil.h"

#define NUM_TYPES 8

class Object {
	public:
		Object(uint8_t *indexTable, uint8_t numObjects, uint8_t *dataTable);
		Object(uint8_t *indexTable, uint8_t numObjects);
		Object(uint8_t *buffer);
		~Object();
		
		uint16_t getDataSize();
		uint16_t getSize();
		static uint8_t typeSize(uint8_t type);
		uint8_t getNumObjects();
		uint8_t objectTypeAt(uint8_t index);
		
		uint16_t writeTo(NetworkWriter writer);
		
		int8_t int8At(uint8_t index);
		bool int8At(uint8_t index, int8_t data);
		int16_t int16At(uint8_t index);
		bool int16At(uint8_t index, int16_t data);
		int32_t int32At(uint8_t index);
		bool int32At(uint8_t index, int32_t data);
		
		uint8_t uint8At(uint8_t index);
		bool uint8At(uint8_t index, uint8_t data);
		uint16_t uint16At(uint8_t index);
		bool uint16At(uint8_t index, uint16_t data);
		uint32_t uint32At(uint8_t index);
		bool uint32At(uint8_t index, uint32_t data);
		
		float floatAt(uint8_t index);
		bool floatAt(uint8_t index, float data);
		
		uint8_t strlenAt(uint8_t index);
		char *strAt(uint8_t index);
		bool strAt(uint8_t index, char *str, uint16_t stringLen);
		
		void setDataBuffer(uint8_t *buffer);
		
		static const uint8_t typesArray[][NUM_TYPES];
		
		enum TYPES {
			T_NONE,
			T_STRING = 0x01,
			T_INT8 = 0x02,
			T_UINT8 = 0x03,
			T_INT16 = 0x04,
			T_UINT16 = 0x05,
			T_INT32 = 0x06,
			T_UINT32 = 0x07,
			T_FLOAT = 0x0C
		};
		
	private:
		uint8_t *indexTable;
		uint8_t numObjects;
		uint8_t *dataTable;
		uint16_t indexOf(uint8_t objIndex);
		void *pointerAt(uint8_t index);
		uint8_t strNum(uint8_t index);
};

#endif
