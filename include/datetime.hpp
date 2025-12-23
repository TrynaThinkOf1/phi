/*

 PHI
 2025/12/22

 Phi C++ Project
 include/datetime.hpp

 Zevi Berlin

*/

#ifndef DATETIME_HPP
#define DATETIME_HPP

#include <ctime>
#include <string>
#include <array>

#define DATELEN 32

namespace phi::time {

inline std::string timeToStr(time_t time) {
  struct tm datetime{};
  if (localtime_r(&time, &datetime) == nullptr) {
    return std::string("Unknown datetime");
  }

  std::array<char, DATELEN> raw{};
  size_t len = strftime(raw.data(), DATELEN, "%d/%m/%Y @ %H:%M", &datetime);

  return std::string(raw.data(), len);
}

inline time_t getCurrentTime() {
  return ::time(0);  // :: is for global namespace, (sometimes) necessary for C funcs
}

}  // namespace phi::time

#endif /* DATETIME_HPP */
