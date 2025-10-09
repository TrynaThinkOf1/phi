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
#include <string>

#include "utils.hpp"

namespace phid {

static int create_db_file() {
  const std::string expanded_path = expand("~/.phi/");

  if (!(std::filesystem::exists(expanded_path))) {
    try {
      std::filesystem::create_directory(expanded_path);
    } catch (std::filesystem::filesystem_error e) {
      std::cout << "Could not access database files. Try giving Phi sudo permissions." << std::endl;
      return -1;
    }
  }

  if (!(std::filesystem::exists(expanded_path + "main.db"))) {
    // no try-catch here because it already passed above
    std::ofstream file(expanded_path + "main.db");

    if (file.is_open()) {
      file.close();
    } else {
      ;
      // TODO: Fatal error handling for daemon
    }
  }

  std::error_code err;
  std::filesystem::permissions(expanded_path, std::filesystem::perms::owner_all,
                               std::filesystem::perm_options::replace, err);

  if (err) {
    std::cout << "Could not access database files. Try giving Phi sudo permissions." << std::endl;
    return -1;
  }

  return 0;
}

}  // namespace phid

#endif /* STARTUP_HPP */
