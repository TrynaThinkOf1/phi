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

#include "termcolor/termcolor.hpp"

#include "phi/database/Database.hpp"
#include "phi/encryption/Encryptor.hpp"
#include "tasks/TaskMaster.hpp"
#include "phi/ui/Manager.hpp"
#include "phi/do_setup.hpp"
#include "utils/misc_utils.hpp"
#include "utils/str_utils.hpp"

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

  const std::shared_ptr<phi::tasks::TaskMaster> TASKMASTER =
    std::make_shared<phi::tasks::TaskMaster>(true, erc);
  if (erc == 1) {
    std::cout
      << tmc::bright_red
      << "⛔️ Failed to load first task from database (~/.phi/tasks.db) due to invalid JSON ⛔️\n"
      << tmc::reset;
    return erc;
  }

  /**** ****/

  auto size = phi::ui::getTerminalSize();
  if (size.ws_col < phi::ui::COLS || size.ws_row < phi::ui::ROWS) {
    std::cout << tmc::bright_red << "⛔️ Phi requires a terminal size of at least "
              << phi::ui::COLS << "x" << phi::ui::ROWS << " to properly render the TUI ⛔️\n"
              << tmc::reset;
    return 1;
  }

  const std::shared_ptr<phi::ui::Manager> MANAGER = std::make_shared<phi::ui::Manager>(DATABASE, ENCRYPTOR, TASKMASTER);

  if (!MANAGER->loginPage()) {
    return 1;
  }

  // TODO: make this with FTXUI instead
  /*
  phi::sysmsg("PLEASE LOG IN\n  password: ");
  std::string password = getHiddenInput();
  if (!DATABASE->login(password)) {
    std::cout << tmc::bright_red
              << "\n"
                 "⛔️ Login failed, incorrect password ⛔️\n"  // "" necessary separator for emoji
              << tmc::reset;
    return erc;
  }
  std::cout << tmc::bold << tmc::bright_green << "\nLogin success!\n" << tmc::reset;
  */
  DATABASE->createTables();

  // MANAGER->event_loop();

  return 0;
}