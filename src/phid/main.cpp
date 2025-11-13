/*

  PHI
  2025/09/20

  Phi C++ Project
  src/phid/main.cpp

  Zevi Berlin

*/

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <csignal>
#include <memory>
#include <exception>
#include <thread>
#include <chrono>

#include <termcolor/termcolor.hpp>

#include "phid/STARTUP.hpp"
#include "Logger.hpp"
#include "phid/database/DBManager.hpp"
#include "phid/encryption/MessageTypes.hpp"
#include "phid/encryption/EncryptionManager.hpp"
#include "phid/encryption/secrets.hpp"
#include "phid/PipelineManager.hpp"
#include "utils.hpp"

namespace tmc = termcolor;

void signal_handler(int signal) {
  std::exit(1);  // NOLINT -- ensure that std::atexit is called
}

void exiting() {
  phid::exitUnique();  // located in include/phid/STARTUP.hpp -- yes, I know
}

void termination() {
  exiting();
  std::abort();
}

int main() {
  std::atexit(exiting);
  std::set_terminate(termination);

  // register signal handlers
  std::signal(SIGINT, signal_handler);   // ^C
  std::signal(SIGTERM, signal_handler);  // request for termination

  /***/

  bool needs_setup = false;

  /***/

  bool scs = false;
  const std::shared_ptr<phi::Logger> LOG =
    std::make_shared<phi::Logger>("~/.phi/phid_errors.log", scs);
  if (!scs) {
    std::cout << tmc::bright_red << "Error creating log file for phid.\n";
    std::cout << "Please refer to online documentation for help on fixing logging issues.\n";
    std::cout << "Error code 2A\n" << tmc::reset;
    return 1;
  }

  /**/

  scs = false;

  if (!phid::createDBFile() || !phid::uniqueChecker()) {
    return 1;
  }  // exit if not valid permissions or not unique

  /**/

  const std::shared_ptr<phid::EncryptionManager> EMAN = std::make_shared<phid::EncryptionManager>();

  const std::shared_ptr<phid::DBManager> DMAN = std::make_shared<phid::DBManager>(scs);
  if (!scs) {
    LOG->log("error", "Database not found, needs setup");
    needs_setup = true;
  }

  scs = false;
  const std::shared_ptr<phid::PipelineManager> PIPE =
    std::make_shared<phid::PipelineManager>(scs, LOG);
  if (!scs) {
    return 1;
  }  // exit if we failed to create a listening pipeline socket

  return 0;
}