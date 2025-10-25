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
#include <sstream>

#include "utils.hpp"
#include "phid/database/DBManager.hpp"
#include "phid/encryption/EncryptionManager.hpp"

namespace phid {

[[nodiscard]] static bool createDBFile() {
  const std::string PATH = expand("~/.phi/");

  if (!(std::filesystem::exists(PATH))) {
    try {
      std::filesystem::create_directory(PATH);
    } catch (std::filesystem::filesystem_error e) {
      return false;
    }
  }

  if (!(std::filesystem::exists(PATH + "main.db"))) {
    // no try-catch here because it already passed above
    std::ofstream file(PATH + "main.db");

    if (file.is_open()) {
      file.close();
    } else {
      return false;
    }
  }

  std::error_code err;
  std::filesystem::permissions(PATH, std::filesystem::perms::owner_all,
                               std::filesystem::perm_options::replace, err);

  return !static_cast<bool>(err);
}

/***/

[[nodiscard]] static bool uniqueChecker() {
  const std::string PATH = expand("~/.phi/unique.var");

  int unique{};

  std::ofstream file(PATH);

  if (file.is_open()) {
    std::stringstream buf{};
    buf << file.rdbuf();
    if (buf.tellp() == 0) {  // checks if it has an available char
      try {
        unique = std::stoi(buf.str());
      } catch (std::invalid_argument e) {
        unique = 2;  // trigger logic down below
      }
    }
  } else {
    return false;
  }

  if (unique == 0) {
    file << 1;
    file.close();

    return true;
  } else if (unique != 1) {  // NOLINT
    file << 0;
  }

  // happens if the unique == 1 || unique != 1
  file.close();
  return false;
}

static void exitUnique() {
  const std::string PATH = expand("~/.phi/unique.var");

  std::ofstream file(PATH);

  if (file.is_open()) {
    file << 0;
    file.close();
  }
}

}  // namespace phid

#endif /* STARTUP_HPP */
