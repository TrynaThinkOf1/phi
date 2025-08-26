/*

 PHI
 2025/08/24

 Phi C++ Project
 include/networking/ListenSocketSpawner.hpp

 Zevi Berlin

*/

#ifndef LISTENSOCKETSPAWNER_HPP
#define LISTENSOCKETSPAWNER_HPP

#include <memory>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "networking/ListenSocket.hpp"

namespace phi {

class ListenSocketSpawner {
  private:
    int sock;
    struct sockaddr_in addr;

    void create_addr(int protocol);

  public:
    ListenSocketSpawner(int protocol);
    ~ListenSocketSpawner();

    std::unique_ptr<phi::ListenSocket> accept();

    void end() {
      close(sock);
    }
};

}  // namespace phi

#endif /* LISTENSOCKETSPAWNER_HPP */
