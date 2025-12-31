#ifndef _UTILS_HPP
#define _UTILS_HPP

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <stdexcept>

#include <termios.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/utsname.h>

#include <curl/curl.h>
#include "termcolor/termcolor.hpp"

namespace tmc = termcolor;

namespace phi {
template <typename T>
inline void sysmsg(T add) {
  std::cout << tmc::italic << tmc::bold << tmc::dark << tmc::magenta << add << tmc::reset;
}
}  // namespace phi

[[nodiscard]] static std::string getHiddenInput() {
  // CODE BELOW IS VIA STACK OVERFLOW {
  termios oldt{};
  tcgetattr(STDIN_FILENO, &oldt);
  termios newt = oldt;
  newt.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  // }
  std::string input;
  std::getline(std::cin, input);
  // CLEANUP {
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  // }

  return input;
}

/**/

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

[[nodiscard]] static std::string toHex(const std::string& str) {
  std::ostringstream oss;
  oss << std::hex << std::setfill('0');
  for (unsigned char character : str) {
    oss << std::setw(2) << static_cast<int>(character);
  }
  return oss.str();
}

[[nodiscard]] static std::string toHex(const unsigned char* str) {
  std::string temp{reinterpret_cast<const char*>(str)};
  return toHex(temp);
}

/***/

static inline unsigned char hexValue(char character) {
  constexpr int OFFSET = 10;

  if (character >= '0' && character <= '9') {
    return static_cast<unsigned char>(character - '0');
  }
  if (character >= 'a' && character <= 'f') {
    return static_cast<unsigned char>(OFFSET + character - 'a');
  }
  if (character >= 'A' && character <= 'F') {
    return static_cast<unsigned char>(OFFSET + character - 'A');
  }

  return '\0';
}

/***/

[[nodiscard]] static std::string fromHex(const std::string& hex) {
  if (hex.size() % 2 != 0) {
    return std::string{};
  }

  std::string out;
  out.reserve(hex.size() / 2);

  for (size_t i = 0; i < hex.size(); i += 2) {
    unsigned char high = hexValue(hex[i]);
    unsigned char low = hexValue(hex[i + 1]);
    out.push_back(static_cast<char>((high << 4) | low));
  }

  return out;
}

[[nodiscard]] static std::string fromHex(const unsigned char* hex) {
  std::string temp(reinterpret_cast<const char*>(hex));
  if (temp.empty()) {
    return std::string{};
  }
  return fromHex(temp);
}

/***/

[[nodiscard]] static std::string getHardwareProfile() {
  std::string profile = "[os: ";

  struct utsname name{};
  if (uname(&name) == -1) {
#if defined(__APPLE__) || defined(__MACH__)
    profile += "Unsure-darwin / arch: ";
#elif defined(__linux__)
    profile += "Unsure-linux / arch: ";
#elif defined(_WIN32) || defined(_WIN64)
    profile += "Unsure-windows / arch: ";
#else
    profile += "Unknown / arch: ";
#endif

#if defined(__x86_64__) || defined(_M_X64)
    profile += "Unsure-x86_64]";
#elif defined(__aarch64__) || defined(_M_ARM64)
    profile += "Unsure-arm64]";
#else
    profile += "Unknown]";
#endif
  } else {
    profile += std::string(name.sysname) + " (v:" + std::string(name.release) + ")";
    profile += " / arch: ";
    profile += name.machine;  // NOLINT -- it's a char* but this is fine and doesn't need changing
    profile += "]";
  }

  return profile;
}

/***/

static size_t curlWriteCb(void* contents, size_t size, size_t nmemb, void* userp) {
  size_t total = size * nmemb;
  std::string* out = static_cast<std::string*>(userp);
  out->append(static_cast<char*>(contents), total);
  return total;
}

[[nodiscard]] static std::string getPublicIPv6() {
  CURL* curl = curl_easy_init();
  if (!curl) return "";

  std::string response;

  curl_easy_setopt(curl, CURLOPT_URL, "http://api6.ipify.org");
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCb);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

  CURLcode res = curl_easy_perform(curl);

  long http_code = 0;
  if (res == CURLE_OK) curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

  curl_easy_cleanup(curl);

  if (res != CURLE_OK || http_code != 200) return "";

  return response;
}

/***/

[[nodiscard]] static bool renameFile(const std::string& path, const std::string& new_path) {
#ifdef _WIN32
  return (_wrename(utf8_to_utf16(path).c_str(), utf8_to_utf16(new_path).c_str()) == 0);
#else
  return (std::rename(path.c_str(), new_path.c_str()) == 0);
#endif
}

[[nodiscard]] static bool createDataFiles(int& erc) {
  /*
    erc: 0 if none, 1 if ~/.phi/ can't be created, 2 if main.db, 3 if self.json, 4 if bus.phi
    permissions
  */

  const std::string PATH = expand("~/.phi/");

  if (!(std::filesystem::exists(PATH))) {
    try {
      std::filesystem::create_directory(PATH);
    } catch (std::filesystem::filesystem_error e) {
      erc = 1;
      return false;
    }
  }

  if (!std::filesystem::exists(PATH + "main.db")) {
    // no try-catch here because it already passed above
    std::ofstream file(PATH + "main.db");

    if (file.is_open()) {
      file.close();
    } else {
      erc = 2;
      return false;
    }
  }

  if (!std::filesystem::exists(PATH + "self.json")) {
    std::ofstream file(PATH + "self.json");

    if (file.is_open()) {
      file.close();
    } else {
      erc = 3;
      return false;
    }
  }

  if (!std::filesystem::exists(PATH + "tasks.db")) {
    std::ofstream file(PATH + "tasks.db");

    if (file.is_open()) {
      file.close();
    } else {
      erc = 4;
      return false;
    }
  }

  erc = 0;
  return true;
}

#endif /* _UTILS_HPP */
