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

#include <termcolor/termcolor.hpp>

#include "phid/STARTUP.hpp"
#include "phid/database/DBManager.hpp"
#include "phid/encryption/MessageTypes.hpp"
#include "phid/encryption/EncryptionManager.hpp"
#include "phid/encryption/secrets.hpp"
#include "utils.hpp"

// namespace tmc = termcolor;

int main(int argc, char** argv) {
  if (phid::create_db_file() == -1) {
    return -1;
  }  // exit if not valid permissions

  return 0;
}