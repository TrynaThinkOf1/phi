/*

  PHI
  2025/09/20

  Phi C++ Project
  src/phid/main.cpp

  Zevi Berlin

*/

#include "phid/STARTUP.hpp"

int main(int argc, char** argv) {
  if (phid::create_db_file() == -1) {
    return 0;
  }  // exit

  return 0;
}
