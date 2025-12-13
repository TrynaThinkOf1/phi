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
#include <cerrno>
#include <stdexcept>

#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/utsname.h>

#include <fmt/format.h>
#include <httplib.h>

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

static bool parseDateTime(const std::string& datetime_str, std::tm& tm_out) {
  std::istringstream stream(datetime_str);
  stream >> std::get_time(&tm_out, "%Y-%m-%d+%H:%M");
  return !stream.fail();
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
    profile += fmt::format("{} (v:{})", name.sysname, name.release);
    profile += " / arch: ";
    profile += name.machine;  // NOLINT -- it's a char* but this is fine and doesn't need changing
    profile += "]";
  }

  return profile;
}

/***/

[[nodiscard]] static std::string getPublicIPv6() {
  httplib::Client cli("http://api6.ipify.org");
  cli.set_follow_location(true);  // follow redirects

  auto res = cli.Get("/");
  if (res && res->status == 200) {  // NOLINT 200 magic number
    return res->body;
  }
  return "";  // no IPv6 available
}

[[nodiscard]] static bool createDataFiles() {
  const std::string PATH = expand("~/.phi/");

  if (!(std::filesystem::exists(PATH))) {
    try {
      std::filesystem::create_directory(PATH);
    } catch (std::filesystem::filesystem_error e) {
      return false;
    }
  }

  if (!std::filesystem::exists(PATH + "main.db")) {
    // no try-catch here because it already passed above
    std::ofstream file(PATH + "main.db");

    if (file.is_open()) {
      file.close();
    } else {
      return false;
    }
  }

  if (!std::filesystem::exists(PATH + "self.json")) {
    std::ofstream file(PATH + "self.json");

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

#endif /* _UTILS_HPP */
