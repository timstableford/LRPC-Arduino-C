#ifndef __RPC_H__
#define __RPC_H__

#include "Object.h"
#include "NetworkUtil.h"

typedef void (*RPCCallback)(void *userdata, Object &obj);

class RPC {
	public:
		typedef struct {
			uint16_t functionID;
			RPCCallback callback;
			void *userdata;
		} RPCContainer;
		RPC(NetworkWriter writer, RPCContainer *rpcs, uint16_t numRPCs, void *userdata);
		~RPC();
		void typeHandlerCallback(uint8_t *buffer, uint16_t size);
		
		uint16_t call(uint16_t functionID, const char *fmt, ...);
		
	private:
		Object::TYPES getType(char c);
		NetworkWriter writer;
		RPCContainer *rpcs;
		uint16_t numRPCs;
		void *userdata;

};

#endif
