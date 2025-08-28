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
#include "phid/error_handling.hpp"
#include "phid/sort_connections.hpp"

void phi::listen_loop() {
  phi::ListenSocketSpawner spawner(AF_INET);

  std::unique_ptr<phi::ListenSocket> sock;
  while (1) {
    sock = spawner.accept();  // internal error handling
    phi::sort_connection(std::move(sock));
  }
  spawner.end();
  return;
}
