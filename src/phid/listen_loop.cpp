/*

 PHI
 2025/08/25

 Phi C++ Project
 src/phid/listen_loop.cpp

 Zevi Berlin

*/

#include "phid/listen_loop.hpp"

#include <atomic>
#include <thread>
#include <chrono>
#include <memory>
#include <iostream>
#include <string>
#include <netinet/in.h>

#include "networking/ListenSocketSpawner.hpp"
#include "networking/ListenSocket.hpp"
#include "phid/error_handling.hpp"
#include "phid/sort_connections.hpp"

void phi::listen_loop(std::atomic<bool>& should_continue) {
  std::cout << "Starting listen loop" << std::endl;
  phi::ListenSocketSpawner spawner(AF_INET);

  std::unique_ptr<phi::ListenSocket> sock;
  while (static_cast<bool>(should_continue)) {
    sock = spawner.accept();
    if (!sock) {                                                    // nullptr
      std::this_thread::sleep_for(std::chrono::milliseconds(100));  // backoff
      continue;
    }

    phi::sort_connection(std::move(sock));
  }

  std::cout << "Ending listen loop" << std::endl;
  spawner.end();
}
