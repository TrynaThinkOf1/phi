#ifndef STR_UTILS_HPP
#define STR_UTILS_HPP


#include <iostream>
#include <string>
#include <cstdio>
#include <sstream>
#include <iomanip>
#include <regex>

#include <termios.h>
#include <unistd.h>

#include <cryptopp/base64.h>

//---------> [ Config. Separator ] <---------\\ 

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

//------------[ Func. Implementation Separator ]------------\\ 

[[nodiscard]] static std::string toB64(const std::string& str) {
  std::string opt;

  CryptoPP::Base64Encoder encoder(nullptr, false);
  encoder.Put(reinterpret_cast<const unsigned char*>(str.data()), str.size());
  encoder.MessageEnd();

  CryptoPP::word64 size = encoder.MaxRetrievable();
  if (size > 0) {
    opt.resize(size);
    encoder.Get(reinterpret_cast<unsigned char*>(opt.data()), size - 1);
  }

  return opt;
}

//------------[ Func. Implementation Separator ]------------\\ 

[[nodiscard]] static std::string fromB64(const std::string& str) {
  std::string opt;

  CryptoPP::Base64Decoder decoder(nullptr);
  decoder.Put(reinterpret_cast<const unsigned char*>(str.data()), str.size());
  decoder.MessageEnd();

  CryptoPP::word64 size = decoder.MaxRetrievable();
  if (size > 0) {
    opt.resize(size);
    decoder.Get(reinterpret_cast<unsigned char*>(opt.data()), size);
  }

  return opt;
}

//------------[ Func. Implementation Separator ]------------\\ 

static const std::regex ipv6_pattern(
  "(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-"
  "9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-"
  "fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-"
  "9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|"
  "fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,"
  "1}[0-9]){0,1}[0-9])\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:(("
  "25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))");

[[nodiscard]] static bool validIPv6(const std::string& str) {
  return std::regex_match(str, ipv6_pattern);
}

//

#include "utils/count_emojis.hpp"

#endif /* STR_UTILS_HPP */