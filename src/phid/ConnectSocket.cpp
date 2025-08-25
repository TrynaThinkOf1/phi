/*

 PHI
 2025/08/24

 Phi C++ Project
 src/shared/ConnectSocket.cpp

 Zevi Berlin

*/

#include "networking/ConnectSocket.hpp"

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

phi::ConnectSocket::ConnectSocket(int protocol, int port) {
  this->sock = socket(protocol, SOCK_STREAM, 0);
}
