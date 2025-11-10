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
#include <regex>
#include <iterator>
#include <ctime>
#include <cstdint>

#include "utils.hpp"

//
size_t findLastDateIndex(const std::string& log_content, const std::string& reference_date_str);

//

/** CONSTRUCTOR & DESCRUCTOR **/
phi::Logger::Logger(const std::string& path, bool& scs) : path(expand(path)) {
  this->file.open(this->path, std::ios::binary | std::ios::app);  // NOLINT -- append mode
  if (!this->file.is_open()) {
    scs = false;
    return;
  }

  std::time_t real_time_struct = time(NULL);
  std::tm real_time_tm{};
#if defined(_MSC_VER)  // windows
  localtime_s(&real_time_tm, &real_time_struct);
#else
  localtime_r(&real_time_struct, &real_time_tm);
#endif

  std::vector<char> buf(DATE_MAXLEN);
  if (std::strftime(buf.data(), buf.size(), "%Y-%m-%d+%H:%M", &real_time_tm) != 0) {
    this->file << "\n[INFO] `Logging started` @ " << buf.data() << " [END INFO]\n";
  } else {
    this->file << "\n[INFO] `Logging started` @ " << "UNKNOWN DATETIME"
               << " [END INFO]\n";
  }

  scs = true;
}

phi::Logger::~Logger() {
  std::time_t real_time_struct = time(NULL);
  std::tm real_time_tm{};
#if defined(_MSC_VER)  // windows
  localtime_s(&real_time_tm, &real_time_struct);
#else
  localtime_r(&real_time_struct, &real_time_tm);
#endif

  std::vector<char> buf(DATE_MAXLEN);
  if (std::strftime(buf.data(), buf.size(), "%Y-%m-%d+%H:%M", &real_time_tm) == 0) {
    std::memcpy(buf.data(), "UNKNOWN DATETIME", 16);  // NOLINT -- magic number
  }
  this->file << "[INFO] `Logging ended` @ " << buf.data() << " [END INFO]\n";

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

  std::time_t last_week_struct = real_time_struct - 604800;  // NOLINT -- 604800 seconds in 1 week
  std::tm last_week_tm{};
#if defined(_MSC_VER)  // windows
  localtime_s(&last_week_tm, &last_week_struct);
#else
  localtime_r(&last_week_struct, &last_week_tm);
#endif

  this->file.close();  // necessary to then open a new ifstream for reading it

  /**/

  std::ifstream inf(this->path, std::ios::binary);
  if (!inf.is_open()) {
    return;  // reading file failed
  }

  std::string contents((std::istreambuf_iterator<char>(inf)), std::istreambuf_iterator<char>());

  inf.close();

  /**/

  std::vector<char> buf(DATE_MAXLEN);
  if (strftime(buf.data(), DATE_MAXLEN, "%Y-%m-%d+%H:%M", &last_week_tm) == 0) {
    return;  // couldn't create the date 1 week ago
  }

  size_t idx = findLastDateIndex(contents, std::string(buf.begin(), buf.end()));
  if (idx == std::string::npos) {
    return;  // no old logs
  }
  idx += DATE_MAXLEN;
  size_t next = contents.substr(idx).find(']');
  idx += (next != std::string::npos ? next + 1 : 0) + 2;  // + 2 = \n\n to clear to next line

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

  std::error_code erc;
  std::filesystem::rename(temp, orig, erc);  // may fail on windows
  if (erc) {
    erc = {};

    std::filesystem::path old(this->path + ".old");
    std::filesystem::rename(orig, old, erc);

    erc = {};
    std::filesystem::rename(temp, orig, erc);

    if (erc) {                                       // still failing, resort to brute force
      std::ofstream logout(orig, std::ios::binary);  // create new logfile
      if (!logout.is_open()) {                       // goodness fucking gracious
        std::filesystem::rename(old, orig, erc);
        return;  // everything failed
      }

      std::ifstream login(old, std::ios::binary);
      if (!login.is_open()) {
        std::filesystem::rename(old, orig, erc);  // this won't fail but also won't clean logs

        logout.close();
        return;
      }

      // it worked, i've been thrown a bone
      logout << login.rdbuf();

      logout.close();
      login.close();

      std::filesystem::remove(old);
      std::filesystem::remove(temp);
    }
  }
}

/** **/

/** PUBLIC METHODS **/

void phi::Logger::log(const std::string& level, const std::string& content) {
  /*
  Add a log line to the log file
   for info or error handling

  Args:
   level - whatever log level
   content - string content for whatever info or error
  */

  std::time_t real_time_struct = time(NULL);
  std::tm real_time_tm{};
#if defined(_MSC_VER)
  localtime_s(&real_time_tm, &real_time_struct);
#else
  localtime_r(&real_time_struct, &real_time_tm);
#endif

  std::vector<char> buf(DATE_MAXLEN);
  if (std::strftime(buf.data(), buf.size(), "%Y-%m-%d+%H:%M", &real_time_tm) == 0) {
    std::memcpy(buf.data(), "UNKNOWN DATETIME", 16);  // NOLINT -- magic number
  }

  this->file << "[" << level << "] `" << content << "` @ " << buf.data() << " [END " << level
             << "]\n";
}

/***/

bool phi::Logger::deleteNumLogs(int num, int& erc) {
  /*
  Deletes num + 1 lines from log
   file to effectively clear some
   log history

  Args:
   num - # lines to delete
  */

  if (num < 0) num = 1;  // NOLINT

  this->file.close();  // close so that I can open an in file

  std::ifstream inf(this->path, std::ios::binary);
  if (!inf.is_open()) {
    this->file.open(this->path, std::ios::binary | std::ios::app);  // append mode
    if (!this->file.is_open()) {
      erc = 2;
      return false;
    }

    erc = 1;
    return false;  // failed to do it
  }

  std::string contents((std::istreambuf_iterator<char>(inf)), std::istreambuf_iterator<char>());

  inf.close();

  this->file.open(this->path, std::ios::binary | std::ios::trunc);  // deletes the file contents
  if (!this->file.is_open()) {
    this->file.open(this->path, std::ios::binary | std::ios::app);  // try to restore it
    if (!this->file.is_open()) {
      erc = 2;
      return false;
    }

    erc = 1;
    return false;  // failed to open output file
  }

  contents.erase(contents.length() - 1);  // erase the last newline

  size_t idx = contents.rfind('\n');
  idx = (idx != std::string::npos ? idx - 1 : contents.length());  // it keeps the \n
  contents.erase(idx);

  this->file << contents;
  this->file.close();

  this->file.open(this->path, std::ios::binary | std::ios::app);
  if (!this->file.is_open()) {
    erc = 2;
    return false;
  }

  return true;  // leave the file open
}

/***/


bool phi::Logger::clearLogs() {
  /*
  Delete entire log file
  */

  this->file.close();

  this->file.open(this->path, std::ios::binary | std::ios::trunc);  // clear file contents
  return this->file.is_open();
}

/** **/

/** NON CLASS METHODS **/
size_t findLastDateIndex(const std::string& log_content, const std::string& reference_date_str) {
  // Regex to match: YYYY-MM-DD+HH:MM
  std::regex date_regex(R"(\d{4}-\d{2}-\d{2}\+\d{2}:\d{2})");
  std::smatch match;

  // Parse reference date
  std::tm ref_tm = {};
  if (!parseDateTime(reference_date_str, ref_tm)) {
    return -1;
  }
  std::time_t ref_time = std::mktime(&ref_tm);

  std::size_t last_pos = std::string::npos;

  std::string::const_iterator search_start = log_content.cbegin();
  while (std::regex_search(search_start, log_content.cend(), match, date_regex)) {
    std::string matched_str = match.str();
    std::tm log_tm = {};
    if (parseDateTime(matched_str, log_tm)) {
      std::time_t log_time = std::mktime(&log_tm);

      // Check if log_time <= ref_time (i.e., older or equal)
      if (difftime(log_time, ref_time) <= 0) {
        last_pos = match.position();  // Update last valid position
      }
    }
    search_start = match.suffix().first;
  }

  return last_pos;  // Returns string index, or npos if none found
}