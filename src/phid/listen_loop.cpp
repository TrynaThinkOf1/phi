/*

 PHI
 2025/08/25

 Phi C++ Project
 src/phid/listen_loop.cpp

 Zevi Berlin

*/

#include "phid/listen_loop.hpp"

#include <iostream>
#include <string>
#include <netinet/in.h>

#include "networking/ListenSocketSpawner.hpp"
#include "networking/ListenSocket.hpp"
#include "Exceptions.hpp"

void phi::listen_loop() {
  phi::ListenSocketSpawner spawner(AF_INET);
  while (1) {
    phi::ListenSocket* sock = nullptr;
    try {
      phi::ListenSocket _new_sock = spawner.accept();
      sock = &_new_sock;
    } catch (phi::FailedSocketError e) {
      std::cerr << e.what() << std::endl;
      break;
    }

    std::string buf;
    buf.reserve(32000);
    sock->recv(buf);
    std::cout << buf << std::endl;
    sock->end();
  }
  spawner.end();
  return;
}
