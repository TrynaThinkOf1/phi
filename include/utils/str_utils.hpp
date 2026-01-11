#ifndef STR_UTILS_HPP
#define STR_UTILS_HPP


#include <iostream>
#include <string>
#include <cstdio>
#include <sstream>
#include <iomanip>

#include <termios.h>
#include <unistd.h>

#include <cryptopp/base64.h>

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

[[nodiscard]] static std::string toB64(const std::string& str) {
  std::string opt;

  CryptoPP::Base64Encoder encoder;
  encoder.Put(reinterpret_cast<const unsigned char*>(str.data()), str.size());
  encoder.MessageEnd();

  CryptoPP::word64 size = encoder.MaxRetrievable();
  if (size > 0) {
    opt.resize(size);
    encoder.Get(reinterpret_cast<unsigned char*>(opt.data()), size);
  }

  return opt;
}

[[nodiscard]] static std::string fromB64(const std::string& str) {
  std::string opt;

  CryptoPP::Base64Decoder decoder;
  decoder.Put(reinterpret_cast<const unsigned char*>(str.data()), str.size());
  decoder.MessageEnd();

  CryptoPP::word64 size = decoder.MaxRetrievable();
  if (size > 0) {
    opt.resize(size);
    decoder.Get(reinterpret_cast<unsigned char*>(opt.data()), size);
  }

  return opt;
}

#endif /* STR_UTILS_HPP */