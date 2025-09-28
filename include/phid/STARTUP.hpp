/*

 PHI
 2025/09/19

 Phi C++ Project
 include/phid/STARTUP.hpp

 Zevi Berlin

*/

#ifndef STARTUP_HPP
#define STARTUP_HPP

#include <filesystem>
#include <fstream>
#include <iostream>

namespace phid {

static int create_db_file() noexcept {
  if (!(std::filesystem::exists("/var/phi"))) {
    try {
      std::filesystem::create_directory("/var/phi");
    } catch (std::filesystem::filesystem_error e) {
      std::cout << "Could not access database files. Try giving Phi sudo permissions." << std::endl;
      return -1;
    }
  }

  if (!(std::filesystem::exists("/var/phi/main.db"))) {
    // no try-catch here because it already passed above
    std::ofstream file("/var/phi/main.db");

    if (file.is_open()) {
      file.close();
    } else {
      ;
      // TODO: Fatal error handling for daemon
    }
  }

  return 0;
}

}  // namespace phid

#endif /* STARTUP_HPP */
