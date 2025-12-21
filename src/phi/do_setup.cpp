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
  const std::string ipv6_addr = "";  // getPublicIPv6();

  std::cout << tmc::blink << tmc::bright_blue << "Generating RSA keys (4096 bytes)...";

  std::string rsa_pub, rsa_priv;
  ENCRYPTOR->rsaGenPair(rsa_pub, rsa_priv);

  std::cout << tmc::reset << tmc::green << "\nGenerated\n" << tmc::reset;

  int erc = 0;
  if (!DATABASE->createSelf(name, emoji, rsa_pub, rsa_priv, ipv6_addr, hardware_profile, erc)) {
    std::cout << tmc::bright_red
              << "⛔️ Could not store self because the self file (~/.phi/self.json) could not be "
                 "accessed ⛔️\n"
              << tmc::reset;
    return false;
  }

  return true;
}