/*

  PHI
  2025/09/20

  Phi C++ Project
  src/phid/main.cpp

  Zevi Berlin

*/

#include <iostream>
#include <string>

#include <termcolor/termcolor.hpp>

#include "phid/STARTUP.hpp"
#include "phid/database/DBManager.hpp"
#include "phid/encryption/EncryptedMessage.hpp"
#include "phid/encryption/EncryptionManager.hpp"
#include "utils.hpp"

namespace tmc = termcolor;

int main(int argc, char** argv) {
  if (phid::create_db_file() == -1) {
    return -1;
  }  // exit if not valid permissions

  int profileExists = 0;
  phid::DBManager dbm = phid::DBManager(profileExists);
  if (profileExists == -1) {
    std::cout
      << tmc::bold << tmc::bright_red
      << "Your local Phi profile hasn't been set up yet. Would you like to set it up now (y/n)? "
      << tmc::reset;
    char input = 0;
    std::cin.read(&input, 1);

    if (input == 'y' || input == 'Y') {
      // dbm.initialize_self();
      dbm.change_self_name("Zevi Berlin");
    } else {
      std::cout << "Exiting..." << '\n';
      return 0;
    }
  }  // exit if self isn't defined

  std::cout << "Welcome, " << dbm.self.name << "!" << '\n';

  return 0;
}
