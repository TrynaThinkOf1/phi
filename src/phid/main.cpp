/*

 PHI
 20-08-2025

 Phi C++ Project
 src/main.cpp

 Zevi Berlin

*/

#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "phid/listen_loop.hpp"

int main(int argc, char** argv) {
  std::unique_ptr<std::vector<std::string>> args =
    std::make_unique<std::vector<std::string>>(
      argv, argv + argc);  // easier to work with vectors

  std::atomic<bool> listener_should_continue{true};

  std::thread listen_thread(phi::listen_loop,
                            std::ref(listener_should_continue));

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  listener_should_continue = false;
  listen_thread.join();

  return 0;
}
