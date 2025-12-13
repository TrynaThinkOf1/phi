/*

 PHI
 2025/09/24

 Phi C++ Project
 include/phi/encryption/MessageTypes.hpp

 Zevi Berlin

*/

#ifndef MESSAGETYPES_HPP
#define MESSAGETYPES_HPP

#include <string>
#include <unordered_map>
#include <cstdint>

#include <sodium.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace phi::encryption {

struct EncryptedMessage {
    uint8_t version;

    std::string content;

    std::string chacha_key;  // RSA encrypted ChaCha20-Poly1305 key
    std::array<unsigned char, crypto_aead_chacha20poly1305_NPUBBYTES> nonce;

    std::string blake2_hash;

    std::string mac;

    /**/

    std::unordered_map<std::string, std::string*> MAP;

    EncryptedMessage() {
      this->MAP["content"] = &this->content;
      this->MAP["chacha_key"] = &this->chacha_key;
      this->MAP["blake2_hash"] = &this->blake2_hash;
      this->MAP["mac"] = &this->mac;
    }

    /***/

    bool is_valid() const {
      return (this->version != 0 && this->content.length() > 0 &&
              this->chacha_key.length() == 512 && this->blake2_hash.length() == 64 &&
              this->mac.length() == 32);
    }

    std::string to_json_str() const {
      json j;

      for (const auto& [field, ptr] : this->MAP) {
        j[field] = *ptr;
      }

      j["version"] = (int)this->version;
      j["nonce"] = this->nonce;

      return j.dump();
    }

    bool from_json_str(const std::string& json_str) {
      if (!json::accept(json_str)) return false;

      json j = json::parse(json_str);

      for (const auto& [field, ptr] : this->MAP) {
        if (j[field] == nullptr) {
          return false;
        }

        *ptr = j[field].get<std::string>();
      }

      if (j["version"] == nullptr) return false;
      this->version = static_cast<uint8_t>(j["version"].get<int>());

      if (j["nonce"] == nullptr) return false;
      std::memcpy(this->nonce.data(), j["nonce"].get<std::string>().data(),
                  crypto_aead_chacha20poly1305_NPUBBYTES);

      return true;
    }
};

struct AuthenticatedUpdate {
    uint8_t version;

    std::string content;

    std::string mac;

    /***/

    bool is_valid() const {
      return (this->version != 0 && this->content.length() > 0 && this->mac.length() == 32);
    }

    std::string to_json_str() const {
      json j;

      j["version"] = static_cast<int>(this->version);
      j["content"] = this->content;
      j["mac"] = this->mac;

      return j.dump();
    }

    bool from_json_str(const std::string& json_str) {
      if (!json::accept(json_str)) return false;

      json j = json::parse(json_str);

      if (j["version"] == nullptr) return false;
      this->version = static_cast<uint8_t>(j["version"].get<int>());

      if (j["content"] == nullptr) return false;
      this->content = j["content"].get<std::string>();

      if (j["mac"] == nullptr) return false;
      this->mac = j["mac"].get<std::string>();

      return true;
    }
};

}  // namespace phi::encryption

#endif /* MESSAGETYPES_HPP */
