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
#include <filesystem>
#include <string>
#include <regex>
#include <iterator>
#include <ctime>
#include <cstdint>

#include "utils.hpp"

namespace phi {

class Logger {
  private:
    std::ofstream file;
    std::string path;

    static constexpr std::size_t DATE_MAXLEN = 64;

    /** HELPER FUNCS **/
    void killOldLogs(const time_t& real_time_struct);
    /** **/

  public:
    /** CONSTRUCTOR & DESCRUCTOR **/
    Logger(const std::string& path, bool& scs);
    ~Logger();
    /** **/

    void log(const std::string& level, const std::string& content);

    bool deleteNumLogs(int num, int& erc);

    bool clearLogs();
};

}  // namespace phi

#endif /* LOGGER_HPP */
