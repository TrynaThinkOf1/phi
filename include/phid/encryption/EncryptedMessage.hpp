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

namespace phid {

struct EncryptedMessage {
    uint8_t version;

    bool is_file;

    std::string content;
    int msg_split = content.length() / 1024;

    std::string encrypted_aes_key;

    std::string blake2_hash_plaintext;

    int nonce;
};

}  // namespace phid

#endif /* ENCRYPTEDMESSAGE_HPP */
