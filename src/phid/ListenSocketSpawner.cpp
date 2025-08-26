/*

 PHI
 2025/08/24

 Phi C++ Project
 src/shared/ListenSocketSpawner.cpp

 Zevi Berlin

*/

#include "networking/ListenSocketSpawner.hpp"

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "networking/ListenSocket.hpp"
#include "Exceptions.hpp"
#include "config.hpp"

// constructor + destructor
phi::ListenSocketSpawner::ListenSocketSpawner(int protocol) {
  sock = socket(protocol, SOCK_STREAM, 0);
  if (sock < 0) {
    throw phi::FailedSocketError(sock);
  }

  create_addr(protocol);

  if (bind(sock, (const struct sockaddr*)&addr, sizeof(addr)) < 0) {
    throw phi::FailedSocketError(sock);
  }

  if (listen(sock, 3) < 0) {  // 3 is backlog
    throw phi::FailedSocketError(sock);
  }
}

phi::ListenSocketSpawner::~ListenSocketSpawner() {
  close(sock);
}

// others
void phi::ListenSocketSpawner::create_addr(int protocol) {
  addr.sin_family = protocol;  // sin_family
  addr.sin_port =
    htons(phi::RECV_PORT);  // sin_port - htons() sets port bytes in right
                            // order (BE vs. LE) for network
  addr.sin_addr.s_addr = INADDR_ANY;  // listen for any connection
}

phi::ListenSocket phi::ListenSocketSpawner::accept() {
  int new_sock;
  struct sockaddr* new_addr = nullptr;

  new_sock = ::accept(sock, new_addr, NULL);
  if (new_sock < 0) {
    throw phi::FailedSocketError(new_sock);
  }

  phi::ListenSocket connected_socket(new_sock, new_addr);
  return connected_socket;
}
