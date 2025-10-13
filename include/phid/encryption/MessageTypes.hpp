/*

 PHI
 2025/09/24

 Phi C++ Project
 include/phid/encryption/MessageTypes.hpp

 Zevi Berlin

*/

#ifndef ENCRYPTEDMESSAGE_HPP
#define ENCRYPTEDMESSAGE_HPP

#include <string>
#include <unordered_map>
#include <cstdint>

#include <sodium.h>
#include <sys/types.h>
#include <nlohmann/json.hpp>

#include "utils.hpp"

using json = nlohmann::json;

namespace phid {

struct EncryptedMessage {
    uint8_t version;

    std::string content;

    std::string chacha_key;  // RSA encrypted ChaCha20-Poly1305 key
    unsigned char nonce[crypto_aead_chacha20poly1305_NPUBBYTES];

    std::string blake2_hash;

    std::string mac = "";

    /**/

    const std::unordered_map<std::string, std::string*> str_field_map{
      {"content", &this->content},
      {"chacha_key", &this->chacha_key},
      {"blake2_hash", &this->blake2_hash},
      {"mac", &this->mac}};

    /***/

    bool is_valid() const {
      return (this->version != 0 && this->content.length() > 0 &&
              this->chacha_key.length() == 512 && this->blake2_hash.length() == 64 &&
              this->mac.length() == 32);
    }

    std::string to_json_str() const {
      json j;

      for (const auto& [field, ptr] : str_field_map) {
        j[field] = toHex(*ptr);
      }

      j["version"] = (int)this->version;
      j["nonce"] = toHex(this->nonce);

      return j.dump();
    }

    bool from_json_str(const std::string& json_str) {
      json j;
      if (json::accept(json_str)) {
        j = json::parse(json_str);
      } else {
        return false;
      }

      for (const auto& [field, ptr] : str_field_map) {
        if (j[field] == nullptr) {
          return false;
        }

        *ptr = fromHex(j[field].get<std::string>());
      }

      if (j["version"] == nullptr) return false;
      this->version = static_cast<uint8_t>(j["version"].get<int>());

      if (j["nonce"] == nullptr) return false;
      std::memcpy(this->nonce, fromHex(j["nonce"].get<std::string>()).data(),
                  crypto_aead_chacha20poly1305_NPUBBYTES);

      return true;
    }
};

struct AuthenticatedUpdate {
    uint8_t version;

    std::string content;

    std::string mac = "";

    /***/

    bool is_valid() const {
      return (this->version != 0 && this->content.length() > 0 && this->mac.length() == 32);
    }

    std::string to_json_str() const {
      json j;

      j["version"] = static_cast<int>(this->version);
      j["content"] = this->content;
      j["mac"] = toHex(this->mac);

      return j.dump();
    }

    bool from_json_str(const std::string& json_str) {
      json j;
      if (json::accept(json_str)) {
        j = json::parse(json_str);
      } else {
        return false;
      }

      if (j["version"] == nullptr) return false;
      this->version = static_cast<uint8_t>(j["version"].get<int>());

      if (j["content"] == nullptr) return false;
      this->content = j["content"].get<std::string>();

      if (j["mac"] == nullptr) return false;
      this->mac = fromHex(j["mac"].get<std::string>());

      return true;
    }
};

}  // namespace phid

#endif /* ENCRYPTEDMESSAGE_HPP */
