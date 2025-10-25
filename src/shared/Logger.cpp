/*

 PHI
 2025/10/25

 Phi C++ Project
 src/shared/Logger.cpp

 Zevi Berlin

*/

#include "Logger.hpp"

#include <fstream>
#include <string>
#include <ctime>
#include <cstdint>

#define DATE_MAXLEN (const int)50

/** CONSTRUCTOR & DESCRUCTOR **/
phi::Logger::Logger(const std::string& path, bool& scs) {
  const std::string PATH = expand(path);

  this->file = new std::ofstream(PATH, std::ios_base::app);  // NOLINT -- app = append
  if (!this->file->is_open()) {
    scs = false;
    return;
  }

  time_t real_time_struct = time(NULL);
  struct tm* real_time = std::localtime(&real_time_struct);  // NOLINT

  std::vector<char> buf(DATE_MAXLEN);
  if (std::strftime(buf.data(), buf.size(), "%Y-%m-%d+%H:%M", real_time) != 0) {
    *(this->file) << "\n[INFO] `Logging started` @ " << buf.data() << " [END INFO]\n";
  } else {
    *(this->file) << "\n[INFO] `Logging started` @ " << "UNKNOWN DATETIME"
                  << " [END INFO]\n";
  }

  scs = true;
}

phi::Logger::~Logger() {
  time_t real_time_struct = time(NULL);
  struct tm* real_time = std::localtime(&real_time_struct);  // NOLINT

  std::vector<char> buf(DATE_MAXLEN);
  if (std::strftime(buf.data(), buf.size(), "%Y-%m-%d+%H:%M", real_time) != 0) {
    *(this->file) << "\n[INFO] `Logging ended` @ " << buf.data() << " [END INFO]\n";
  } else {
    *(this->file) << "\n[INFO] `Logging ended` @ " << "UNKNOWN DATETIME"
                  << " [END INFO]\n";
  }

  this->killOldLogs(real_time_struct);

  this->file->close();
  delete this->file;
}

/** **/

/** HELPER FUNCS **/

void phi::Logger::killOldLogs(const time_t& real_time_struct) {
  /*
  Take the current log file and find any logs
   that are more than 1 week old and delete them
   so that the log file can stay clean
  */

  time_t last_week_struct = real_time_struct - 604800;       // NOLINT -- 604800 seconds in 1 week
  struct tm* last_week = std::localtime(&last_week_struct);  // NOLINT

  std::vector<char> buf(DATE_MAXLEN);
  std::strftime(buf.data(), buf.size(), "%Y-%m-%d+%H:%M", last_week);

  std::string last_week_ts(buf.begin(), buf.end());
}

/** **/

void phi::Logger::log(phi::LogLevel level, const std::string& content) {
  /*
  Add a log line to the log file
   for info or error handling

  Args:
   level - whatever log level
   content - string content for whatever info or error
  */

  time_t real_time_struct = time(NULL);
  struct tm* real_time = std::localtime(&real_time_struct);  // NOLINT

  std::vector<char> buf(DATE_MAXLEN);
  std::strftime(buf.data(), buf.size(), "%Y-%m-%d+%H:%M", real_time);

  switch (level) {
    case phi::LogLevel::INFO:
      *(this->file) << "[INFO] `" << content << "` @ " << buf.data() << " [END INFO]\n";
      break;
    case phi::LogLevel::WARNING:
      *(this->file) << "[WARNING] `" << content << "` @ " << buf.data()
                    << " [WARNING INFO]\n";
      break;
    case phi::LogLevel::ERROR:
      *(this->file) << "[ERROR] `" << content << "` @ " << buf.data()
                    << " [ERROR INFO]\n";
      break;
    case phi::LogLevel::CRITICAL:
      *(this->file) << "[CRITICAL] `" << content << "` @ " << buf.data()
                    << " [CRITICAL INFO]\n";
      break;
  }
}