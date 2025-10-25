/*

 PHI
 2025/10/25

 Phi C++ Project
 include/Logger.hpp

 Zevi Berlin

*/

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>
#include <string>
#include <ctime>
#include <cstdint>

#include "utils.hpp"

namespace phid {

enum LEVEL : std::uint8_t { INFO, WARNING, ERROR, CRITICAL };

class Logger {
  private:
    std::ofstream* file;

  public:
    /** CONSTRUCTOR & DESCRUCTOR **/
    Logger(const std::string& path, bool& scs);
    ~Logger();
    /** **/

    void log(LEVEL level, const std::string& content);
};

}  // namespace phid

#endif /* LOGGER_HPP */
