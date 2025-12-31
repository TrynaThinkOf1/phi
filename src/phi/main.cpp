/*

  PHI
  2025/09/20

  Phi C++ Project
  src/phi/main.cpp

  Zevi Berlin

*/

#include <iostream>
#include <string>
#include <memory>
#include <map>

#include "termcolor/termcolor.hpp"

#include "phi/database/Database.hpp"
#include "phi/encryption/Encryptor.hpp"
#include "tasks/TaskMaster.hpp"
#include "tasks/task_struct.hpp"
#include "phi/do_setup.hpp"
#include "utils.hpp"

namespace tmc = termcolor;

int main() {
  /**** GLOBAL CONFIG ****/

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
      case 4:
        std::cout << "⛔️ Failed to create tasks file (~/.phi/tasks.db) ⛔️\n";
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
    std::cout << tmc::yellow << "⚠️ Profile not found, needs setup ⚠️\n" << tmc::reset;

    std::cout << "Would you like to set up your profile right now (y/n)? ";
    char conf{};
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

  ENCRYPTOR->changePrivKey(DATABASE->self.rsa_priv_key);

  /**/

  std::shared_ptr<phi::tasks::TaskMaster> TASKMASTER =
    std::make_shared<phi::tasks::TaskMaster>(true, erc);
  if (erc == 1) {
    std::cout
      << tmc::bright_red
      << "⛔️ Failed to load first task from database (~/.phi/tasks.db) due to invalid JSON ⛔️\n"
      << tmc::reset;
    return erc;
  }

  /**** ****/

  // TODO: LOGIN FOR DATABASE (entails moving the table creation out of constructor)

  // phi::event_loop(DATABASE, ENCRYPTOR, TASKMASTER);

  std::cout << tmc::italic << tmc::bold << tmc::dark << tmc::magenta << "Goodbye!\n" << tmc::reset;

  return 0;
}