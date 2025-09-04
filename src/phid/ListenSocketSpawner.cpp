/*

 PHI
 2025/08/24

 Phi C++ Project
 src/shared/ListenSocketSpawner.cpp

 Zevi Berlin

*/

#include "networking/ListenSocketSpawner.hpp"

#include <memory>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "networking/ListenSocket.hpp"
#include "phid/error_handling.hpp"
#include "config.hpp"

// constructor + destructor
phi::ListenSocketSpawner::ListenSocketSpawner(int protocol) {
  sock = socket(protocol, SOCK_STREAM, 0);
  if (sock < 0) {
    phi::handle_error((std::string) "Error creating socket: " +
                      (std::string)strerror(errno));
  }

  int flags = fcntl(sock, F_GETFL, 0);       // make it non-blocking
  fcntl(sock, F_SETFL, flags | O_NONBLOCK);  // ^

  create_addr(protocol);

  if (bind(sock, (const struct sockaddr*)&addr, sizeof(addr)) < 0) {
    phi::handle_error((std::string) "Error creating socket: " +
                      (std::string)strerror(errno));
  }

  if (listen(sock, 3) < 0) {  // 3 is backlog
    phi::handle_error((std::string) "Error listening on socket: " +
                      (std::string)strerror(errno));
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

std::unique_ptr<phi::ListenSocket> phi::ListenSocketSpawner::accept() {
  std::shared_ptr<struct sockaddr_storage> new_addr =
    std::make_shared<struct sockaddr_storage>();

  socklen_t addrlen = sizeof(new_addr);

  int new_sock = ::accept(sock, (struct sockaddr*)new_addr.get(), &addrlen);
  if (new_sock < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      // No pending connection right now — caller should retry / re-check flag
      return nullptr;
    }
    phi::handle_error((std::string) "Error accepting socket: " +
                      (std::string)strerror(errno));
  }

  return std::make_unique<phi::ListenSocket>(new_sock, new_addr);
}
