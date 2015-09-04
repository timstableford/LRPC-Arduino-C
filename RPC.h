#ifndef __RPC_H__
#define __RPC_H__

#include "Object.h"
#include "NetworkUtil.h"

class RPC {
	public:
		RPC(NetworkWriter writer);
		~RPC();
		
		uint16_t call(uint16_t functionID, const char *fmt, ...);
		
	private:
		Object::TYPES getType(char c);
		NetworkWriter writer;

};

#endif
