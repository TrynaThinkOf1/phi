/*

 PHI
 2025/12/07

 Phi C++ Project
 include/phi/database/structs.hpp

 Zevi Berlin

*/

#ifndef DATABASE_STRUCTS_HPP
#define DATABASE_STRUCTS_HPP

#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include "utils.hpp"

#define LINE_LIM (const int)32

using json = nlohmann::json;

namespace phi::database {

struct self_t {
    std::string name;
    std::string emoji;
    std::string rsa_pub_key;
    std::string rsa_priv_key;
    std::string last_known_ip;
    std::string hardware_profile;

    std::unordered_map<std::string, std::string*> MAP;

    self_t() {
      this->MAP["name"] = &this->name;
      this->MAP["emoji"] = &this->emoji;
      this->MAP["rsa_pub_key"] = &this->rsa_pub_key;
      this->MAP["rsa_priv_key"] = &this->rsa_priv_key;
      this->MAP["last_known_ip"] = &this->last_known_ip;
      this->MAP["hardware_profile"] = &this->hardware_profile;
    }

    /***/

    std::string to_json_str() const {
      json j;

      for (const auto& [field, ptr] : this->MAP) {
        if (field.substr(0, 3) == "rsa") {
          j[field] = toHex(*ptr);
          continue;
        }
        j[field] = *ptr;
      }

      return j.dump();
    }

    bool from_json_str(const std::string& json_str) {
      if (!json::accept(json_str)) return false;

      json j = json::parse(json_str);

      for (const auto& [field, ptr] : this->MAP) {
        if (field.substr(0, 3) == "rsa") {
          *ptr = fromHex(j[field].get<std::string>());
          continue;
        }
        *ptr = j[field].get<std::string>();
      }

      return true;
    }

    std::string toString() const {
      return "NAME: " + this->name + "\nEMOJI: " + this->emoji +
             "\nRSA PUB KEY (first 32): " + toHex(this->rsa_pub_key.substr(0, LINE_LIM)) +
             "\nRSA PRIV KEY (first 32): " + toHex(this->rsa_priv_key.substr(0, LINE_LIM)) +
             "\nIP: " + this->last_known_ip + "\nHARDWARE PROFILE: " + this->hardware_profile;
    }
};

struct contact_t {
    int id;
    std::string name;
    std::string emoji;
    std::string addr;
    std::string rsa_key;
    std::string shared_secret;

    std::unordered_map<std::string, std::string*> MAP;

    contact_t() {
      this->MAP["name"] = &this->name;
      this->MAP["emoji"] = &this->emoji;
      this->MAP["addr"] = &this->addr;
      this->MAP["rsa_key"] = &this->rsa_key;
      this->MAP["shared_secret"] = &this->shared_secret;
    }

    /***/

    std::string to_json_str() const {
      json j;

      for (const auto& [field, ptr] : this->MAP) {
        if (field == "rsa_key") {
          j[field] = toHex(*ptr);
          continue;
        }
        j[field] = *ptr;
      }

      j["id"] = this->id;

      return j.dump();
    }

    bool from_json_str(const std::string& json_str) {
      if (!json::accept(json_str)) return false;

      json j = json::parse(json_str);

      for (const auto& [field, ptr] : this->MAP) {
        if (field == "rsa_key") {
          *ptr = fromHex(j[field].get<std::string>());
          continue;
        }
        *ptr = j[field].get<std::string>();
      }

      this->id = j["id"].get<int>();

      return true;
    }

    std::string toString() const {
      return "ID: " + std::to_string(this->id) + "\nNAME: " + this->name +
             "\nEMOJI: " + this->emoji + "\nSHARED SECRET: " + toHex(this->shared_secret) +
             "\nIP: " + this->addr +
             "\nRSA KEY (first 32): " + toHex(this->rsa_key.substr(0, LINE_LIM));
    }
};

struct message_t {
    int id;
    int contact_id;
    bool sender;
    std::string content;
    std::string timestamp;
    bool delivered;

    /***/

    std::string to_json_str() const {
      json j;

      j["id"] = this->id;
      j["contact_id"] = this->contact_id;
      j["sender"] = this->sender;
      j["content"] = this->content;
      j["timestamp"] = this->timestamp;
      j["delivered"] = this->delivered;

      return j.dump();
    }

    bool from_json_str(const std::string& json_str) {
      if (!json::accept(json_str)) return false;

      json j = json::parse(json_str);

      this->id = j["id"].get<int>();
      this->contact_id = j["contact_id"].get<int>();
      this->sender = j["sender"].get<bool>();
      this->content = j["content"].get<std::string>();
      this->timestamp = j["timestamp"].get<std::string>();
      this->delivered = j["delivered"].get<bool>();

      return true;
    }

    std::string toString() const {
      return "ID: " + std::to_string(this->id) + "CONTACT ID: " + std::to_string(this->contact_id) +
             "SENDER: " + (sender ? "true" : "false") + "\nCONTENT: " + this->content +
             "\nTIMESTAMP: " + this->timestamp + "\nDELIVERED: " + (delivered ? "true" : "false");
    }
};

struct error_t {
    int id;
    std::string title;
    std::string description;
    std::string timestamp;

    /***/

    std::string to_json_str() const {
      json j;

      j["id"] = this->id;
      j["title"] = this->title;
      j["description"] = this->description;
      j["timestamp"] = this->timestamp;

      return j.dump();
    }

    bool from_json_str(const std::string& json_str) {
      if (!json::accept(json_str)) return false;

      json j = json::parse(json_str);

      this->id = j["id"].get<int>();
      this->title = j["title"].get<std::string>();
      this->description = j["description"].get<std::string>();
      this->timestamp = j["timestamp"].get<std::string>();

      return true;
    }

    std::string toString() const {
      return "ID: " + std::to_string(this->id) + "\nTITLE: " + this->title +
             "\nDESCRIPTION: " + this->description + "\nTIMESTAMP: " + this->timestamp;
    }
};

}  // namespace phi::database

#endif /* DATABASE_STRUCTS_HPP */