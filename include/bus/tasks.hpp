/*

 PHI
 2025/12/26

 Phi C++ Project
 include/phi/bus/tasks.hpp

 Zevi Berlin

*/

#ifndef TASKS_HPP
#define TASKS_HPP

#include <string>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace phi::bus {

struct task_t {
    int code;
    json data;

    std::string to_json_str() const {
      json j;
      j["code"] = this->code;
      j["data"] = this->data;

      return j.dump();
    }

    bool from_json_str(const std::string& json_str) {
      if (!json::accept(json_str)) return false;

      json j = json::parse(json_str);

      if (j["code"] == nullptr || j["data"] == nullptr) return false;
      this->code = j["code"].get<int>();

      const std::string raw = j["data"].get<std::string>();
      if (!json::accept(raw)) return false;
      this->data = json::parse(raw);

      return true;
    }

    std::string toString() const {
      return "CODE: " + std::to_string(this->code) + "\nDATA: " + this->data.dump();
    }
};

}  // namespace phi::bus

#endif /* TASKS_HPP */