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

int main(int argc, char** argv) {
  std::unique_ptr<std::vector<std::string>> args =
      std::make_unique<std::vector<std::string>>(
        argv, argv + argc);  // easier to work with vectors
        
  return 0;
}
