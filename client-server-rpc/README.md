# Client-server RPC communication
Implements remote procedure call (RPC) communication on top of the temperature server.

RPC is implemented using the JSON-RPC 2.0 spec., and using the nlohmann/json library.

The server runs a threaded service, including
- socket-server
- temperature monitor
- temperature controller (PD-type).

Communicate with the server using a socket client capable of sending JSON-encoded messages or directly via telnet.

Cross-compiles to PocketBeagle using a linaro toolchain (gcc-linaro-7.4.1-2019.02-x86_64_arm-linux-gnueabihf) and Eclipse.
