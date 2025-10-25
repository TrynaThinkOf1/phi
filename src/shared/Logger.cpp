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

/** CONSTRUCTOR & DESCRUCTOR **/
phid::Logger::Logger(const std::string& path, bool& scs) {
  const std::string PATH = expand(path);

  this->file = new std::ofstream(PATH, std::ios_base::app);  // NOLINT -- app = append
  if (!this->file->is_open()) {
    scs = false;
    return;
  }

  time_t timestamp = time(NULL);

  *(this->file) << "\n[INFO] `Logging started` @ " << timestamp << " [END INFO]\n";

  scs = true;
}

phid::Logger::~Logger() {
  time_t timestamp = time(NULL);

  *(this->file) << "\n[INFO] `Logging ended` @ " << timestamp << " [END INFO]\n";
  this->file->close();
  delete this->file;
}

/** **/

void phid::Logger::log(phid::LEVEL level, const std::string& content) {
  time_t timestamp = time(NULL);

  switch (level) {
    case phid::LEVEL::INFO:
      *(this->file) << "[INFO] `" << content << "` @ " << timestamp << " [END INFO]\n";
      break;
    case phid::LEVEL::WARNING:
      *(this->file) << "[WARNING] `" << content << "` @ " << timestamp << " [WARNING INFO]\n";
      break;
    case phid::LEVEL::ERROR:
      *(this->file) << "[ERROR] `" << content << "` @ " << timestamp << " [ERROR INFO]\n";
      break;
    case phid::LEVEL::CRITICAL:
      *(this->file) << "[CRITICAL] `" << content << "` @ " << timestamp << " [CRITICAL INFO]\n";
      break;
  }
}