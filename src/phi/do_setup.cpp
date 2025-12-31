/*

 PHI
 2025/12/13

 Phi C++ Project
 src/phi/do_setup.cpp

 Zevi Berlin

*/

#include "phi/do_setup.hpp"

#include <iostream>
#include <memory>
#include <string>

#include "termcolor/termcolor.hpp"

#include "phi/database/Database.hpp"
#include "phi/encryption/Encryptor.hpp"
#include "utils.hpp"

namespace tmc = termcolor;

bool do_setup(const std::shared_ptr<phi::database::Database>& DATABASE,
              const std::shared_ptr<phi::encryption::Encryptor>& ENCRYPTOR) {
  // TODO: make this with FTXUI instead
  std::cin.ignore();
  phi::sysmsg("\nWhat is your name (default 'Me')? ");
  std::string name;
  std::getline(std::cin, name);
  if (name.empty()) name = "Me";

  phi::sysmsg("Which emoji best represents you (default '🪞')? ");
  std::string emoji;
  std::getline(std::cin, emoji);
  if (emoji.empty() || emoji.length() > 1) emoji = "🪞";

  /*  phi::sysmsg("Create a strong password (IRRECOVERABLE IF LOST, next is hint): ");
    std::string password = getHiddenInput();
    while (password.length() < 5) {
      std::cout << tmc::red << "Password length must be ≥ 5\n" << tmc::reset;
      phi::sysmsg("Create a strong password: ");
      password = getHiddenInput();
    }
  */
  // phi::sysmsg("Short hint for password: ");
  std::string hint = "Default password hint";
  // std::getline(std::cin, hint);

  const std::string hardware_profile = getHardwareProfile();
  const std::string ipv6_addr;  // = getPublicIPv6();

  std::cout << tmc::blink << tmc::magenta << "Generating RSA keys (4096 bytes)...";

  std::string rsa_pub, rsa_priv;
  ENCRYPTOR->rsaGenPair(rsa_pub, rsa_priv);

  std::cout << tmc::reset << tmc::green << "\nGenerated\n" << tmc::reset;
  /*
    if (!DATABASE->login(password)) {
      std::cout << tmc::bright_red
                << "⛔️ The database seems to be locked with a different password already... Are you
    " "sure this is your first time using phi? ⛔️\n"
                << tmc::reset;
      return false;
    }
  */
  int erc = 0;
  if (!DATABASE->createSelf(name, emoji, hint, rsa_pub, rsa_priv, ipv6_addr, hardware_profile,
                            erc)) {
    std::cout << tmc::bright_red
              << "⛔️ Could not store self because the self file (~/.phi/self.json) could not be "
                 "accessed ⛔️\n"
              << tmc::reset;
    return false;
  }

  std::cout << tmc::bold << tmc::bright_green << "\nProfile created successfully!\n" << tmc::reset;

  return true;
}