#ifndef MISC_UTILS_HPP
#define MISC_UTILS_HPP


#include <string>
#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <sys/utsname.h>

#include <curl/curl.h>
#include "termcolor/termcolor.hpp"

namespace tmc = termcolor;

[[nodiscard]] static std::string getHardwareProfile() {
  std::string profile = "[os: ";

  struct utsname name{};
  if (uname(&name) == -1) {
#if defined(__APPLE__) || defined(__MACH__)
    profile += "Unsure-darwin / arch: ";
#elif defined(__linux__)
    profile += "Unsure-linux / arch: ";
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
    profile += std::string(name.sysname) + " (v:" + std::string(name.release) + ")" +
               " / arch: " + name.machine + "]";
  }

  return profile;
}

/**/

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

namespace phi {
template <typename T>
inline void sysmsg(T add) {
  std::cout << tmc::italic << tmc::bold << tmc::dark << tmc::magenta << add << tmc::reset;
}
}  // namespace phi

#endif /* MISC_UTILS_HPP */