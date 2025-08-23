/*

 PHI
 2025/08/23

 Phi C++ Project
 src/phi/networking/SimpleSocket.cpp

 Zevi Berlin

*/

#include "networking/SimpleSocket.hpp"

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Exceptions.hpp"

phi::SimpleSocket::SimpleSocket(int domain, int service, int protocol, int port,
                                unsigned long interface) {
}
