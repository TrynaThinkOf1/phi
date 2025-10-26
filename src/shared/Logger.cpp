/*

 PHI
 2025/10/25

 Phi C++ Project
 src/shared/Logger.cpp

 Zevi Berlin

*/

#include "Logger.hpp"

#include <fstream>
#include <filesystem>
#include <string>
#include <iterator>
#include <ctime>
#include <cstdint>

#include "utils.hpp"

#define DATE_MAXLEN (const int)50

/** CONSTRUCTOR & DESCRUCTOR **/
phi::Logger::Logger(const std::string& path, bool& scs) {
  this->path = expand(path);

  this->file.open(this->path, std::ios::binary | std::ios::app);  // NOLINT -- append mode
  if (!this->file.is_open()) {
    scs = false;
    return;
  }

  time_t real_time_struct = time(NULL);
  struct tm* real_time = localtime_s(&real_time_struct);

  std::vector<char> buf(DATE_MAXLEN);
  if (std::strftime(buf.data(), buf.size(), "%Y-%m-%d+%H:%M", real_time) != 0) {
    this->file << "\n[INFO] `Logging started` @ " << buf.data() << " [END INFO]\n";
  } else {
    this->file << "\n[INFO] `Logging started` @ " << "UNKNOWN DATETIME"
               << " [END INFO]\n";
  }

  scs = true;
}

phi::Logger::~Logger() {
  time_t real_time_struct = time(NULL);
  struct tm* real_time = std::localtime(&real_time_struct);  // NOLINT

  std::vector<char> buf(DATE_MAXLEN);
  if (std::strftime(buf.data(), buf.size(), "%Y-%m-%d+%H:%M", real_time) != 0) {
    this->file << "\n[INFO] `Logging ended` @ " << buf.data() << " [END INFO]\n";
  } else {
    this->file << "\n[INFO] `Logging ended` @ " << "UNKNOWN DATETIME"
               << " [END INFO]\n";
  }

  this->killOldLogs(real_time_struct);  // closes file
}

/** **/

/** HELPER FUNCS **/

void phi::Logger::killOldLogs(const time_t& real_time_struct) {
  /*
  Take the current log file and find any logs
   that are more than 1 week old and delete them
   so that the log file can stay clean
  */

  time_t last_week_struct = real_time_struct - 604800;  // NOLINT -- 604800 seconds in 1 week
  std::tm last_week_tm{};
#if defined(_MSC_VER)  // windows
  localtime_s(&last_week_tm, &last_week_struct);
#else
  localtime_r(&last_week_struct, &last_week_tm);
#endif

  std::vector<char> buf(DATE_MAXLEN);
  if (std::strftime(buf.data(), buf.size(), "%Y-%m-%d+%H:%M", &last_week_tm) == 0) {
    return;  // format failed
  }

  this->file.close();  // necessary to then open a new ifstream for reading it

  /**/

  std::ifstream inf(this->path, std::ios::binary);
  if (!inf.is_open()) {
    return;  // reading file failed
  }

  std::string contents((std::istreambuf_iterator<char>(inf)), std::istreambuf_iterator<char>());

  inf.close();

  /**/

  size_t idx = contents.rfind(buf.data());
  if (idx == std::string::npos) {
    return;  // no old logs
  }

  std::filesystem::path orig(this->path);
  std::filesystem::path temp = orig;
  temp += ".tmp";

  std::ofstream outf(temp, std::ios::binary | std::ios::trunc);
  if (!outf.is_open()) {
    return;  // failed to open temp file
  }

  outf << contents.substr(idx);
  outf.flush();
  outf.close();

  /*
  first try to replace the original with the temporary
  which is supposed to be atomic on POSIX compliant systems.
  if that fails, remove the target
  */
  std::error_code erc;
  std::filesystem::rename(temp, orig, erc);
  if (erc) {
    std::filesystem::remove(temp);
  }
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
      this->file << "[INFO] `" << content << "` @ " << buf.data() << " [END INFO]\n";
      break;
    case phi::LogLevel::WARNING:
      this->file << "[WARNING] `" << content << "` @ " << buf.data() << " [WARNING INFO]\n";
      break;
    case phi::LogLevel::ERROR:
      this->file << "[ERROR] `" << content << "` @ " << buf.data() << " [ERROR INFO]\n";
      break;
    case phi::LogLevel::CRITICAL:
      this->file << "[CRITICAL] `" << content << "` @ " << buf.data() << " [CRITICAL INFO]\n";
      break;
  }
}