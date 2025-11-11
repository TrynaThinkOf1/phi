/*

  PHI
  2025/09/20

  Phi C++ Project
  src/phi/main.cpp

  Zevi Berlin

*/

#include <iostream>
#include <string>
#include <exception>
#include <memory>

#include <termcolor/termcolor.hpp>

#include "Logger.hpp"
#include "phi/Pipeline.hpp"

namespace tmc = termcolor;

int main(int argc, char** argv) {
  bool scs = false;
  const std::shared_ptr<phi::Logger> LOG =
    std::make_shared<phi::Logger>("~/.phi/phi_errors.log", scs);
  if (!scs) {
    std::cout << tmc::bright_red << "Error creating log file for phi.\n";
    std::cout << "Please refer to online documentation for help on fixing logging issues.\n";
    std::cout << "Error code 1A\n" << tmc::reset;
    return 1;
  }

  int erc = -1;
  try {
    const std::shared_ptr<phi::Pipeline> PHID = std::make_shared<phi::Pipeline>(erc);
    PHID->disconnect();
  } catch (std::exception e) {
    switch (erc) {
      case 0:
        std::cout << tmc::bright_red << "Failed to create socket pipeline to daemon.\n";
        std::cout << "Please refer to online documentation for help on socket failure.\n";
        std::cout << "EC: 1B\n" << tmc::reset;
        break;

      case 1:
        std::cout << tmc::bright_red << "Failed to connect to daemon through pipeline socket.\n";
        std::cout << "Please refer to online documentation for help on starting daemon.\n";
        std::cout << "EC: 1C\n" << tmc::reset;
        break;

      default:
        std::cout << tmc::bright_red << "Unknown error occurred!\n" << tmc::reset;
        break;
    }
    return 1;
  }

  return 0;
}
