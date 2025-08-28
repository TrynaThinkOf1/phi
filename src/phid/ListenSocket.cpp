/*

 PHI
 2025/08/24

 Phi C++ Project
 src/phid/ListenSocket.cpp

 Zevi Berlin

*/

#include "networking/ListenSocket.hpp"

#include <memory>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "phid/error_handling.hpp"

void phi::ListenSocket::recv(std::string& buffer) {
  buffer.resize(32000);

  ssize_t n = ::recv(this->sock, buffer.data(), buffer.size(), 0);

  if (n > 0) {
    buffer.resize((size_t)n);  // keep only received bytes
    return;
  }

  if (n == 0) {  // peer closed the connection
    buffer.clear();
    return;
  }

  // n == -1
  if (errno == EINTR)
    return recv(buffer);  // retry on interrupt
  if (errno == EAGAIN || errno == EWOULDBLOCK) {
    // non-blocking and no data, caller handle retry
  }
  phi::handle_error((std::string) "Error receiving information from socket: " +
                    (std::string)strerror(errno));
}

void phi::ListenSocket::send(const std::string& buffer) {
  const char* _buf = buffer.data();

  size_t remaining = buffer.size();

  while (remaining > 0) {
    ssize_t n = ::send(this->sock, _buf, remaining, 0);

    if (n > 0) {
      remaining -= (size_t)n;
      continue;  // effective early return to avoid error handling down there
    }

    if (n == 0)
      phi::handle_error((std::string) "Error sending information to socket: " +
                        (std::string)strerror(errno));  // treat it as an error

    // n == -1
    if (errno == EINTR)
      continue;  // retry on interrupt
    if (errno == EAGAIN || errno == EWOULDBLOCK)
      continue;  // shouldn't happen for this socket, check anyway

    phi::handle_error((std::string) "Error sending information to socket: " +
                      (std::string)strerror(errno));
  }
  return;
}
