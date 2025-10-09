/*

 PHI
 2025/09/24

 Phi C++ Project
 include/phid/encryption/EncryptedMessage.hpp

 Zevi Berlin

*/

#ifndef ENCRYPTEDMESSAGE_HPP
#define ENCRYPTEDMESSAGE_HPP

#include <string>
#include <cstdint>

#include <sodium.h>

namespace phid {

struct EncryptedMessage {
    uint8_t version;

    std::string content;

    std::string chacha_key;  // RSA encrypted ChaCha20-Poly1305 key
    unsigned char nonce[crypto_aead_chacha20poly1305_NPUBBYTES];

    std::string blake2_hash;

    bool is_valid() const {
      return (this->version != 0 && this->content.length() > 0 &&
              this->chacha_key.length() == 512 && this->blake2_hash.length() == 64);
    }
};

}  // namespace phid

#endif /* ENCRYPTEDMESSAGE_HPP */
