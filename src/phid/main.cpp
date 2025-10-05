/*

  PHI
  2025/09/20

  Phi C++ Project
  src/phid/main.cpp

  Zevi Berlin

*/

#include <iostream>
#include <string>

#include "utils.hpp"
#include "phid/STARTUP.hpp"
#include "phid/database/DBManager.hpp"
#include "phid/encryption/EncryptedMessage.hpp"
#include "phid/encryption/EncryptionManager.hpp"

int main(int argc, char** argv) {
  if (phid::create_db_file() == -1) {
    return 0;
  }  // exit if not valid permissions

  return 0;
}
