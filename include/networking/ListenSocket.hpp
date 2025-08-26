/*

 PHI
 2025/08/24

 Phi C++ Project
 include/networking/ListenSocket.hpp

 Zevi Berlin

*/

#ifndef LISTENSOCKET_HPP
#define LISTENSOCKET_HPP

#include <memory>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace phi {

class ListenSocket {
  private:
    int sock;
    std::shared_ptr<struct sockaddr_storage> conn_addr;

  public:
    ListenSocket(int sock_, std::shared_ptr<struct sockaddr_storage> conn_addr_)
        : sock(sock_), conn_addr(conn_addr_) {
    }

    ~ListenSocket() {
      close(sock);
    }

    void recv(std::string& buffer);
    void send(const std::string& buffer);

    void end() {
      close(sock);
    }

    std::shared_ptr<struct sockaddr_storage> get_conn_addr() {
      return conn_addr;
    }
};

}  // namespace phi

#endif /* LISTENSOCKET_HPP */
