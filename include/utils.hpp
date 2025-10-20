#ifndef _UTILS
#define _UTILS

#include <string>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <cstring>
#include <cerrno>
#include <stdexcept>

#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/utsname.h>

#include <fmt/format.h>

static std::string expand(const std::string& path) {
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

static std::string toHex(const std::string& str) {
  std::ostringstream oss;
  oss << std::hex << std::setfill('0');
  for (unsigned char character : str) {
    oss << std::setw(2) << static_cast<int>(character);
  }
  return oss.str();
}

static std::string toHex(const unsigned char* str) {
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

static std::string fromHex(const std::string& hex) {
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

static std::string fromHex(const unsigned char* hex) {
  std::string temp(reinterpret_cast<const char*>(hex));
  if (temp.empty()) {
    return std::string{};
  }
  return fromHex(temp);
}

/***/

static std::string getHardwareProfile() {
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

#endif /* _UTILS */