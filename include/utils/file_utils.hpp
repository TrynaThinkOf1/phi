#ifndef FILE_UTILS_HPP
#define FILE_UTILS_HPP


#include <string>
#include <cstdlib>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <map>

#include <pwd.h>
#include <unistd.h>

[[nodiscard]] static std::string expand(const std::string& path) {
  std::string result{path};

  if (path[0] == '~') {
    std::string home(std::getenv("HOME"));  // NOLINT -- "theoretically" thread safe (cppreference)
    std::string temp(getpwuid(getuid())->pw_dir);  // NOLINT ^

    if (!home.empty()) {
      result = home + path.substr(1);
    }
    if (!temp.empty()) {
      result = temp + path.substr(1);
    }
  }

  return result;
}

/***/

[[nodiscard]] static bool renameFile(const std::string& path, const std::string& new_path) {
  return (std::rename(path.c_str(), new_path.c_str()) == 0);
}

/***/


/*
  erc: 0 if none, 1 if ~/.phi/ can't be created, 2 if main.db, 3 if self.json, 4 if bus.phi
  permissions
*/
[[nodiscard]] static bool createDataFiles(int& erc) {
  const std::string PATH = expand("~/.phi/");

  if (!(std::filesystem::exists(PATH))) {
    try {
      std::filesystem::create_directory(PATH);
    } catch (std::filesystem::filesystem_error e) {
      erc = 1;
      return false;
    }
  }

  const std::map<std::string, int> data{
    {PATH + "main.db", 2}, {PATH + "self.json", 3}, {PATH + "tasks.db", 4}};
  for (const auto& [file_path, error_code] : data) {
    if (!std::filesystem::exists(file_path)) {
      // no try-catch here because it already passed above
      std::ofstream file(file_path);  // RAII -- auto closes

      if (!file) {
        erc = 2;
        return false;
      }
    }
  }

  erc = 0;
  return true;
}


#endif /* FILE_UTILS_HPP */