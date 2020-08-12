# Client-server temperature controller
Implements client-server communication via sockets (posix).

The server runs a threaded service, including
- socket-server
- temperature monitor
- temperature controller (PD-type).

Communicate with the server using a regular socket client or directly via telnet.

Cross-compiles to PocketBeagle using a linaro toolchain (gcc-linaro-7.4.1-2019.02-x86_64_arm-linux-gnueabihf) and Eclipse.
