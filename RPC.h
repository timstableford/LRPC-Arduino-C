#ifndef __RPC_H__
#define __RPC_H__

#include "Object.h"
#include "NetworkUtil.h"
#include "StreamParser.h"

class RPC;
typedef void (*RPCCallback)(RPC &rpc, Object &obj, void *userdata);

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
		void setHandlers(RPCContainer *rpcs, uint16_t numRPCs);
    const StreamParser::TypeHandler *getHandler();
		
		uint16_t call(uint16_t functionID, const char *fmt, ...);
		
    static void typeHandlerWrapper(void *userdata, uint8_t *buffer, uint16_t size) {
      ((RPC *)userdata)->typeHandlerCallback(buffer, size);
    }
	private:
		Object::TYPES getType(char c);
		NetworkWriter writer;
		RPCContainer *rpcs;
		uint16_t numRPCs;
		void *userdata;
    StreamParser::TypeHandler handler;

};

#endif
