/*

 PHI
 2025/08/24

 Phi C++ Project
 src/shared/ListenSocketSpawner.cpp

 Zevi Berlin

*/

#include "networking/ListenSocketSpawner.hpp"

#include <variant>
#include <sys/socket.h>
#include <netinet/in.h>

#include "networking/ListenSocket.hpp"

// constructor + destructor
phi::ListenSocketSpawner::ListenSocketSpawner(int protocol) {
  sock = socket(protocol, SOCK_STREAM, 0);
  if (sock < 0) {
    throw phi::FailedSocketError(sock);
  }

  create_addr(protocol);

  if (bind(sock, (const struct sockaddr*)&addr, sizeof(addr)); < 0) {
    throw phi::FailedSocketError(sock);
  }

  if (listen(sock, 1) < 0) {
    throw phi::FailedSocketError(sock);
  }
}

phi::ListenSocketSpawner::~ListenSocketSpawner() {
  close(sock);
}

// others
void phi::ListenSocketSpawner::create_addr(int protocol) {
  addr.sin_family = protocol;    // sin_family
  addr.sin_port = htons(phi::);  // sin_port - htons() sets port bytes in right
                                 // order (BE vs. LE) for network
  addr.sin_addr.s_addr = INADDR_ANY;  // listen for any connection
}

phi::ListenSocket phi::ListenSocketSpawner::accept() {
}

void phi::ListenSocketSpawner::end() {
  close(sock);
}
