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

    unsigned char chacha_key[crypto_aead_chacha20poly1305_KEYBYTES];
    unsigned char nonce[crypto_aead_chacha20poly1305_NPUBBYTES];

    std::string blake2_hash;
};

}  // namespace phid

#endif /* ENCRYPTEDMESSAGE_HPP */
