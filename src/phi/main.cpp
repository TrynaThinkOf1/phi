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
#include <map>

#include "termcolor/termcolor.hpp"

#include "phi/database/Database.hpp"
#include "phi/encryption/MessageTypes.hpp"
#include "phi/encryption/Encryptor.hpp"
#include "phi/encryption/secrets.hpp"
#include "phi/do_setup.hpp"
#include "utils.hpp"

namespace tmc = termcolor;

int main() {
  int erc = 0;

  if (!createDataFiles(erc)) {
    std::cout << tmc::bright_red;
    switch (erc) {
      case 1:
        std::cout << "⛔️ Failed to create data file directory (~/.phi/) ⛔️\n";
        break;
      case 2:
        std::cout << "⛔️ Failed to create database file (~/.phi/main.db) ⛔️\n";
        break;
      case 3:
        std::cout << "⛔️ Failed to create profile holder (~/.phi/self.json) ⛔️\n";
        break;
      default:
        break;
    }
    std::cout << tmc::reset;
    return erc;
  }  // exit if not valid permissions

  /**/

  const std::shared_ptr<phi::encryption::Encryptor> ENCRYPTOR =
    std::make_shared<phi::encryption::Encryptor>();

  /**/

  const std::shared_ptr<phi::database::Database> DATABASE =
    std::make_shared<phi::database::Database>(erc);
  if (erc == 1) {
    std::cout << tmc::bright_red
              << "⛔️ Failed to create self file (~/.phi/self.json) due to invalid permissions ⛔️\n"
              << tmc::reset;
    return erc;
  } else if (erc == 2) {
    std::cout << tmc::yellow << "⚠️ Profile not found, needs setup ⚠️\n" << tmc::reset;

    std::cout << "Would you like to set up your profile right now (y/n)? ";
    char conf;
    std::cin >> conf;

    if (conf == 'y') {
      if (!do_setup(DATABASE, ENCRYPTOR)) {
        std::cout << tmc::bright_red << "Setup failed." << tmc::reset << "\n";
        return erc;
      }
    } else {
      std::cout << tmc::red << "\nExiting without creating profile.\n" << tmc::reset;
      return 0;
    }
  }

  /**/

  /* TESTING PORTION */

  /* Self -- working
  std::string pub, priv;
  ENCRYPTOR->rsaGenPair(pub, priv);

  std::map<std::string, std::string> self_tests{
    {"name", "Zevi Berlin"}, {"emoji", "🫩"}, {"rsa_pub_key", pub}, {"rsa_priv_key", priv}};

  std::cout << "CURRENT SELF: " << DATABASE->self.toString() << "\n";

  for (const auto& [key, value] : self_tests) {
    if (!DATABASE->changeSelfAttribute(key, value, erc)) {
      if (erc == 1) {
        std::cout << tmc::bright_red << "Couldn't access self!\n" << tmc::reset;
        return erc;
      }
    }

    std::cout << "NEW SELF: " << DATABASE->self.toString() << "\n";
  }
  */

  /* Contacts -- testing
  */

  /* Messages -- testing
  */

  /* Errors -- testing
  */

  /* */

  return 0;
}