/*

 PHI
 2025/08/25

 Phi C++ Project
 src/phid/listen_loop.cpp

 Zevi Berlin

*/

#include "phid/listen_loop.hpp"

#include <memory>
#include <iostream>
#include <string>
#include <netinet/in.h>

#include "networking/ListenSocketSpawner.hpp"
#include "networking/ListenSocket.hpp"
#include "Exceptions.hpp"

void phi::listen_loop() {
  phi::ListenSocketSpawner spawner(AF_INET);

  std::unique_ptr<phi::ListenSocket> sock;
  while (1) {
    try {
      sock = spawner.accept();
    } catch (phi::FailedSocketError e) {
      std::cerr << e.what() << std::endl;
      spawner.end();
      break;
    }

    std::string buf;
    buf.reserve(32000);
    try {
      sock->recv(buf);
    } catch (phi::RecvFailedError e) {
      std::cerr << e.what() << std::endl;
      sock->end();
      spawner.end();
      break;
    }
    std::cout << buf << std::endl;
    sock->end();
  }
  spawner.end();
  return;
}
