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

namespace phi {

struct EncryptedMessage {
    const uint8_t version;

    const bool is_file;

    const std::string content;
    const int msg_split = content.length() / 1024;

    const std::string encrypted_aes_key;
}

}  // namespace phi

#endif /* ENCRYPTEDMESSAGE_HPP */
