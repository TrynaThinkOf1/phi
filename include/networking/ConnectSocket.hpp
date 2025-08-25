/*

 PHI
 2025/08/24

 Phi C++ Project
 include/networking/ConnectSocket.hpp

 Zevi Berlin

*/

#ifndef CONNECTSOCKET_HPP
#define CONNECTSOCKET_HPP

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

namespace phi {

class ConnectSocket {
  private:
    int sock;
    struct sockaddr_in addr;

  public:
    ConnectSocket(int protocol, int port);
};

}  // namespace phi

#endif /* CONNECTSOCKET_HPP */
