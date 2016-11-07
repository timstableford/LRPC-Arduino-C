Implementation of a lightweight RPC protocol in C++ for Linux and Arduino.

Included is an Arduino example called rpc_ping this is designed to be used with https://github.com/timstableford/LRPC-Java
Run ./gradlew runPing example while an Arduino is running this sketch.

It also includes Linux compilation and an example and some tests. It can be compiled into a shared and static library with make and running 'make test' will create a binary called 'lwrpc_tests'.
