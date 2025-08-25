/*

 PHI
 2025/08/24

 Phi C++ Project
 include/networking/ListenSocket.hpp

 Zevi Berlin

*/

#ifndef LISTENSOCKET_HPP
#define LISTENSOCKET_HPP

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

namespace phi {

class ListenSocket {
  private:
    int sock;
    const struct sockaddr conn_addr;

  public:
    ListenSocket(int sock_, const struct sockaddr conn_addr_)
        : sock(sock_) conn_addr(conn_addr_) {
    }

    ~ListenSocket() {
      close(sock);
    }

    void recv(std::string& buf);
    void send(const std::string& buf);

    void end() {
      close(sock);
    }

    const struct sockaddr get_conn_addr() {
      return conn_addr;
    }
};

}  // namespace phi

#endif /* LISTENSOCKET_HPP */
