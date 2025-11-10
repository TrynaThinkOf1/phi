/*

  PHI
  2025/09/20

  Phi C++ Project
  src/phid/main.cpp

  Zevi Berlin

*/

#include <iostream>
#include <string>
#include <cstring>
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
#include "phid/Pipeline.hpp"
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
    std::cout << tmc::bright_red << "ERROR CREATING ERROR LOG FOR PHID!\n" << tmc::reset;
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
    LOG->log("ERROR", "Database not found, needs setup");
    needs_setup = true;
  }

  const std::shared_ptr<phid::Pipeline> PHI = std::make_shared<phid::Pipeline>();
  std::this_thread::sleep_for(std::chrono::seconds(10));
  if (PHI->isConnected()) {
    PHI->send("HELLO, PIPELINE!");
    std::cout << "message sent\n";
    std::this_thread::sleep_for(std::chrono::seconds(10));
    std::cout << "message received: " << PHI->read(1024) << "\n";
    PHI->disconnect();
  }

  return 0;
}