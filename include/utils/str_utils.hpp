#ifndef STR_UTILS_HPP
#define STR_UTILS_HPP


#include <iostream>
#include <string>
#include <cstdio>
#include <sstream>
#include <iomanip>

#include <termios.h>
#include <unistd.h>

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

#endif /*STR_UTILS_HPP */