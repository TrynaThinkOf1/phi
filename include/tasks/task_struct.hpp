/*

 PHI
 2025/12/26

 Phi C++ Project
 include/phi/tasks/task_struct.hpp

 Zevi Berlin

*/

#ifndef TASK_STRUCT_HPP
#define TASK_STRUCT_HPP

#include <string>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace phi::tasks {

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

    bool data_from_json_str(const std::string& data_str) {
      if (!json::accept(data_str)) return false;
      this->data = json::parse(data_str);

      return true;
    }

    std::string data_to_json_str() const {
      return this->data.dump();
    }

    std::string toString() const {
      return "CODE: " + std::to_string(this->code) + "\nDATA: " + this->data.dump() + "\n";
    }
};

}  // namespace phi::tasks

#endif /* TASK_STRUCT_HPP */