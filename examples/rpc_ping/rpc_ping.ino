#include <StreamParser.h>
#include <RPC.h>
#include <Object.h>

#define BUFFER_SIZE 128
uint8_t buffer[BUFFER_SIZE];

int64_t time_offset = 0;

uint16_t serialWriter(void *userdata, uint8_t *data, uint16_t length) {
  return (uint16_t) Serial.write(data, length);
}

int16_t serialReader(void *userdata) {
  return (int16_t) Serial.read();
}

void rpcPing(RPC &rpc, Object &obj, void *userdata) {
  time_offset = obj.int64At(1) - millis();
  // Respond pong.
  rpc.call(2, "m", (int64_t)millis() + time_offset);
}

void rpcPong(RPC &rpc, Object &obj, void *userdata) {
  time_offset = obj.int64At(1) - millis();
}

RPC::RPCContainer rpcs[] = {
  { 1, rpcPing, NULL },
  { 2, rpcPong, NULL }
};

RPC rpc(serialWriter, rpcs, sizeof(rpcs) / sizeof(RPC::RPCContainer), NULL);
StreamParser parser(serialReader, buffer, (uint16_t)128, rpc.getHandler(), 1, NULL);

void setup() {
  Serial.begin(115200);
  // Send initial ping to say we're alive.
  rpc.call(1, "m", (int64_t)0);
}

void loop() {
  while(parser.parse() >= 0);
}
