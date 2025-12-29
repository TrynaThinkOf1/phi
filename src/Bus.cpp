/*

 PHI
 2025/12/26

 Phi C++ Project
 include/phi/bus/Bus.hpp

 Zevi Berlin

*/

#include "bus/Bus.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

#include "nlohmann/json.hpp"

#include "bus/tasks.hpp"
#include "utils.hpp"

// we (I) have decided that this is the best way to do resource
// checking, rather than using something like poll()
#define STDSLEEP std::this_thread::sleep_for(std::chrono::milliseconds(10))

using json = nlohmann::json;

void phi::bus::Bus::_getFirstTask(int& erc) {
  while (!std::filesystem::exists(this->PATH)) {
    STDSLEEP;
  }

  if (!renameFile(this->PATH, this->LOCK_PATH)) {
    erc = 2;
    return;
  }

  task_t task{};

  std::ifstream in(this->LOCK_PATH);
  if (!in) {
    erc = 1;
    return false;
  }
  std::stringstream raw;
  raw << in.rdbuf();
  in.close();
  std::string content = raw.str();

  bool scs = false;
  while (!scs) {
    if (content.empty()) {
      erc = 0;
      if (!renameFile(this->LOCK_PATH, this->PATH)) erc = 2;
      return;
    }

    // reverse operations so that I get the first task and cut it
    scs = task.from_json_str(content.substr(0, content.find('\n') + 1));
    content = content.substr(content.find('\n') + 1);
  }

  std::ofstream out(this->LOCK_PATH);
  if (!out) {
    erc = 1;
    return false;
  }

  out << content;
  out.close();

  if (!renameFile(this->LOCK_PATH, this->PATH)) {
    erc = 2;
    return;
  }

  erc = 0;
  this->current_task = task;
}

/***/

bool phi::bus::Bus::nextTask(bool prev_scs, int& erc) {
  while (!std::filesystem::exists(this->PATH)) {
    STDSLEEP;
  }

  if (!renameFile(this->PATH, this->LOCK_PATH)) {
    erc = 2;
    return false;
  }


  task_t task{};
  if (prev_scs) {
    std::ifstream in(this->LOCK_PATH);
    if (!in) {
      erc = 1;
      return false;
    }
    std::stringstream raw;
    raw << in.rdbuf();
    in.close();
    std::string content = raw.str();

    if (this->backup > 0) this->backup--;

    bool scs = false;
    while (!scs) {
      if (content.empty()) {
        erc = 3;
        if (!renameFile(this->LOCK_PATH, this->PATH)) erc = 2;
        return false;
      }

      content = content.substr(content.find('\n') + 1);
      scs = task.from_json_str(content.substr(0, content.find('\n') + 1));
    }

    std::ofstream out(this->LOCK_PATH);
    if (!out) {
      erc = 1;
      return false;
    }

    out << content;
    out.close();
  } else {
    this->backup++;
  }


  if (!renameFile(this->LOCK_PATH, this->PATH)) {
    erc = 2;
    return false;
  }

  erc = 0;
  return true;
}

bool phi::bus::Bus::addTask(const task_t& task, int& erc) {
}