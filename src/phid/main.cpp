/*

 PHI
 20-08-2025

 Phi C++ Project
 src/main.cpp

 Zevi Berlin

*/

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "phid/listen_loop.hpp"

int main(int argc, char** argv) {
  std::unique_ptr<std::vector<std::string>> args =
    std::make_unique<std::vector<std::string>>(
      argv, argv + argc);  // easier to work with vectors

  std::cout << "This is phid" << std::endl;

  phi::listen_loop();

  return 0;
}
