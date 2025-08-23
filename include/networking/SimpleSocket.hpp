/*

 PHI
 2025/08/23

 Phi C++ Project
 include/networking/SimpleSocket.hpp

 Zevi Berlin

*/

#ifndef SIMPLESOCKET_HPP
#define SIMPLESOCKET_HPP

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Exceptions.hpp"

namespace phi {

class SimpleSocket {
  protected:
    struct sockaddr_in addr;
    int sock;

  public:
    SimpleSocket(int domain, int service, int protocol, int port,
                 unsigned long interface);
    ~SimpleSocket() = default;
};

}  // namespace phi

#endif /* SIMPLESOCKET_HPP */
