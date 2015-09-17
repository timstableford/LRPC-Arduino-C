#ifndef __STREAM_PARSER_H__
#define __STREAM_PARSER_H__

#include "NetworkUtil.h"

#define TYPE_FUNCTION_CALL 8

typedef void (*TypeHandlerCallback)(void *userdata, uint8_t *buffer, uint16_t size);

class StreamParser {
	public:
		enum ParserState {
			PS_IDLE,
			PS_PARSING
		};
		
		typedef struct {
			uint16_t type;
			uint16_t size;
			uint16_t crc;
		} __attribute__((packed)) PacketHeader;
		
		typedef struct {
			uint16_t type;
			TypeHandlerCallback callback;
			void *userdata;
		} TypeHandler;
	
		StreamParser(NetworkReader serialReader,
			uint8_t *buffer,
			uint16_t bufferSize,
			TypeHandler *handlers,
			uint8_t numTypeHandlers,
			void *userdata);
		~StreamParser();
		int16_t parse();
		
		static uint16_t crc16(uint8_t *data_p, uint16_t length);
		static PacketHeader makePacket(uint16_t type, uint16_t size);
		static bool parseHeader(PacketHeader &ph);
	private:
		NetworkReader serialReader;
		PacketHeader packetHeader;
		ParserState state;
		void setState(ParserState st);
		uint8_t *buffer;
		uint16_t bufferSize;
		uint16_t bufferIndex;
		TypeHandler *handlers;
		uint8_t numTypeHandlers;
		void *userdata;
};

#endif
