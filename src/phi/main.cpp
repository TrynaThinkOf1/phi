/*

  PHI
  2025/09/20

  Phi C++ Project
  src/phi/main.cpp

  Zevi Berlin

*/

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <exception>
#include <thread>
#include <csignal>

#include <termcolor/termcolor.hpp>

#include "phi/database/Database.hpp"
#include "phi/encryption/MessageTypes.hpp"
#include "phi/encryption/Encryptor.hpp"
#include "phi/encryption/secrets.hpp"
#include "utils.hpp"

namespace tmc = termcolor;


bool do_setup(const std::shared_ptr<phi::database::Database>& DATABASE);

int main() {
  if (!createDataFiles()) {
    std::cout << tmc::bright_red
              << "⛔️ Failed to create database file (~/.phi/main.db) due to invalid permissions ⛔️"
              << tmc::reset << "\n";
    return 1;
  }  // exit if not valid permissions

  /**/

  int erc = 0;

  const std::shared_ptr<phi::encryption::Encryptor> ENCRYPTOR =
    std::make_shared<phi::encryption::Encryptor>();

  const std::shared_ptr<phi::database::Database> DATABASE =
    std::make_shared<phi::database::Database>(erc);
  if (erc == 1) {
    std::cout << tmc::bright_red
              << "⛔️ Failed to create self file (~/.phi/self.json) due to invalid permissions ⛔️"
              << tmc::reset << "\n";
    return 1;
  } else if (erc == 2) {
    std::cout << tmc::yellow << "⚠️  Database entry for self not found, needs setup ⚠️ " << tmc::reset
              << "\n";

    if (!do_setup(DATABASE)) {
      std::cout << tmc::bright_red << "Setup failed." << tmc::reset << "\n";
      return 1;
    }
  }

  return 0;
}

bool do_setup(const std::shared_ptr<phi::database::Database>& DATABASE) {
  return true;
}