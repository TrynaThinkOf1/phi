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


bool do_setup(const std::shared_ptr<phi::database::Database>& DATABASE,
              const std::shared_ptr<phi::encryption::Encryptor>& ENCRYPTOR);

/***/

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
    return 1;
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
    return 1;
  } else if (erc == 2) {
    std::cout << tmc::yellow << "⚠️ Profile not found, needs setup ⚠️\n" << tmc::reset;

    std::cout << "Would you like to set up your profile right now (y/n)? ";
    char conf;
    std::cin >> conf;

    if (conf == 'y') {
      if (!do_setup(DATABASE, ENCRYPTOR)) {
        std::cout << tmc::bright_red << "Setup failed." << tmc::reset << "\n";
        return 1;
      }
    } else {
      std::cout << tmc::red << "\nExiting without creating profile.\n" << tmc::reset;
      return 1;
    }
  }

  std::cout << "\n\nPROFILE LOADED/CREATED:\n";
  std::cout << DATABASE->self.toString();
  std::cout << "\n";

  return 0;
}

/***/

bool do_setup(const std::shared_ptr<phi::database::Database>& DATABASE,
              const std::shared_ptr<phi::encryption::Encryptor>& ENCRYPTOR) {
  std::cin.ignore();
  std::cout << tmc::bright_blue << "\nWhat is your name (default 'Me')? " << tmc::reset;
  std::string name;
  std::getline(std::cin, name);
  if (name.empty()) name = "Me";

  std::cout << tmc::bright_blue << "Which emoji best represents you (default '🪞')? " << tmc::reset;
  std::string emoji;
  std::getline(std::cin, emoji);
  if (emoji.empty() || emoji.length() > 1) emoji = "🪞";

  const std::string hardware_profile = getHardwareProfile();
  // const std::string ipv6_addr = getPublicIPv6();
  std::cout << tmc::italic << tmc::yellow
            << "Im skipping retreiving IPv6 address because I know neither of us have access.\n"
            << tmc::reset;

  std::cout << tmc::blink << tmc::bright_blue << "Generating RSA keys (4096 bytes)...";

  std::string rsa_pub, rsa_priv;
  ENCRYPTOR->rsaGenPair(rsa_pub, rsa_priv);

  std::cout << tmc::reset << tmc::green << "\nGenerated\n" << tmc::reset;

  int erc = 0;
  if (!DATABASE->createSelf(name, emoji, rsa_pub, rsa_priv, "", hardware_profile, erc)) {
    std::cout << tmc::bright_red
              << "⛔️ Could not store self because the self file (~/.phi/self.json) could not be "
                 "accessed ⛔️\n"
              << tmc::reset;
    return false;
  }

  return true;
}