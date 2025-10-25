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

namespace phi {

enum LogLevel : std::uint8_t { INFO, WARNING, ERROR, CRITICAL };

class Logger {
  private:
    std::ofstream* file;

    /** HELPER FUNCS **/
    void killOldLogs(const time_t& real_time_struct);
    /** **/

  public:
    /** CONSTRUCTOR & DESCRUCTOR **/
    Logger(const std::string& path, bool& scs);
    ~Logger();
    /** **/

    void log(LogLevel level, const std::string& content);
};

}  // namespace phi

#endif /* LOGGER_HPP */
