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
  if (path[0] == '~') {
    const char* home = getenv("HOME");
    if (home || ((home = getpwuid(getuid())->pw_dir))) {
      return std::string(home) + std::string(path.c_str() + 1);
    } else {
      return path;  // return unexpanded if no home found
    }
  } else {
    return path;
  }
}

/***/

static std::string toHex(const std::string& s) {
  std::ostringstream oss;
  oss << std::hex << std::setfill('0');
  for (unsigned char c : s)
    oss << std::setw(2) << static_cast<int>(c);
  return oss.str();
}

static std::string toHex(const unsigned char* s) {
  std::string _s{reinterpret_cast<const char*>(s)};
  return toHex(_s);
}

/***/

static inline unsigned char hexValue(char c) {
  if (c >= '0' && c <= '9') return static_cast<unsigned char>(c - '0');
  if (c >= 'a' && c <= 'f') return static_cast<unsigned char>(10 + c - 'a');
  if (c >= 'A' && c <= 'F') return static_cast<unsigned char>(10 + c - 'A');
  throw std::invalid_argument("Invalid hex character");
}

/***/

static std::string fromHex(const std::string& hex) {
  if (hex.size() % 2 != 0) throw std::invalid_argument("Hex string must have even length");
  std::string out;
  out.reserve(hex.size() / 2);
  for (size_t i = 0; i < hex.size(); i += 2) {
    unsigned char hi = hexValue(hex[i]);
    unsigned char lo = hexValue(hex[i + 1]);
    out.push_back(static_cast<char>((hi << 4) | lo));
  }
  return out;
}

static std::string fromHex(const unsigned char* hex) {
  if (!hex) return std::string{};
  return fromHex(std::string(reinterpret_cast<const char*>(hex)));
}

/***/

static std::string getHardwareProfile() {
  std::string profile = "[os: ";

  struct utsname name;
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
    profile += name.machine;
    profile += "]";
  }

  return profile;
}

#endif /* _UTILS */