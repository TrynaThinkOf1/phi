#ifndef _UTILS
#define _UTILS

#include <string>
#include <sstream>
#include <iomanip>

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

#endif /* _UTILS */