#include "StreamParser.h"
#include "NetworkUtil.h"
#include "Object.h"

StreamParser::StreamParser(NetworkReader serialReader,
		uint8_t *buffer,
		uint16_t bufferSize,
		TypeHandler *handlers,
		uint8_t numTypeHandlers,
		void *userdata)
{
	this->serialReader = serialReader;
	this->buffer = buffer;
	this->bufferSize = bufferSize;
	this->bufferIndex = 0;
	this->state = PS_IDLE;
	this->handlers = handlers;
	this->numTypeHandlers = numTypeHandlers;
	this->userdata = userdata;
}

StreamParser::~StreamParser() {
	
}

StreamParser::PacketHeader StreamParser::makePacket(uint16_t type, uint16_t size) {
	PacketHeader ph;
	ph.type = htons(type);
	ph.size = htons(size);
	ph.crc = crc16((uint8_t *)(&ph), (uint16_t)(sizeof(ph) - sizeof(ph.crc)));
	
	return ph;
}

bool StreamParser::checkHeader(StreamParser::PacketHeader &ph) {
	if(crc16((uint8_t *)(&ph), (uint16_t)(sizeof(ph) - sizeof(ph.crc))) != ph.crc) {
		return false;
	}
	
	ph.type = ntohs(ph.type);
	ph.size = ntohs(ph.size);
	
	return true;
}

int16_t StreamParser::parse() {
	int16_t readByte = this->serialReader(this->userdata);
	if(readByte >= 0) {
		switch(this->state) {
			case PS_IDLE:
			{
				for(uint8_t i = 0; i < sizeof(PacketHeader) - 1; i++) {
					((uint8_t *)(&(this->packetHeader)))[i] = ((uint8_t *)(&(this->packetHeader)))[i + 1];
				}
				((uint8_t *)(&(this->packetHeader)))[sizeof(PacketHeader) - 1] = (uint8_t)readByte;

				if(checkHeader(this->packetHeader)) {
					this->setState(PS_PARSING);
				}
				break;
			}
			case PS_PARSING:
				if(this->packetHeader.size > this->bufferSize) {
					this->setState(PS_IDLE);
					break;
				}
				this->buffer[this->bufferIndex] = (uint8_t)readByte;
				this->bufferIndex++;

				if((this->bufferIndex + 1) > this->packetHeader.size) {
					this->setState(PS_IDLE);
					for(uint8_t i = 0; i < this->numTypeHandlers; i++) {
						if(this->handlers[i].type == this->packetHeader.type) {
							this->handlers[i].callback(this->handlers[i].userdata, this->buffer, this->bufferIndex + 1);
							break;
						}
					}
				}
				
				
				break;
		}
		
	}
	
	return readByte;
}

void StreamParser::setState(ParserState st) {
	this->state = st;
	switch(this->state) {
		case PS_PARSING:
			this->bufferIndex = 0;
			break;
		default:
			break;
	}
}

uint16_t StreamParser::crc16(uint8_t *data_p, uint16_t length) {
    uint16_t crc = crcInit();

    while (length--){
        crc = crcAppend(crc, *data_p++);
    }
    return crc;
}

inline uint16_t StreamParser::crcInit() {
	return 0xFFFF;
}

inline uint16_t StreamParser::crcAppend(uint16_t crc, uint8_t data) {
	uint8_t x = crc >> 8 ^ data;
	x ^= x >> 4;
	return (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
}
