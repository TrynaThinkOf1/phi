/*

 PHI
 2025/09/24

 Phi C++ Project
 include/phid/encryption/EncryptionManager.hpp

 Zevi Berlin

*/

#ifndef ENCRYPTIONMANAGER_HPP
#define ENCRYPTIONMANAGER_HPP

#include <string>
#include <cstdint>

#include <zlc/gzipcomplete.hpp>
#include <cryptopp/cryptlib.h>
#include <cryptopp/blake2.h>

#include "phid/encryption/EncryptedMessage.hpp"

typedef uint8_t byte;

namespace phid {

class EncryptionManager {
  private:
    zlibcomplete::GZipCompressor* compressor =
      new zlibcomplete::GZipCompressor(3, zlibcomplete::auto_flush);
    zlibcomplete::GZipDecompressor* decompressor =
      new zlibcomplete::GZipDecompressor();

    void compress_text(const std::string& text, std::string& op);
    void decompress_text(const std::string& text, std::string& op);

    //

    std::string chacha_key;
    uint8_t num_uses;

    void chacha_encrypt_text(const std::string& text, int& op_nonce,
                             std::string& op_text);

    void chacha_decrypt_text(const std::string& text,
                             const std::string& decrypt_key, const int& nonce,
                             std::string& op_text);

    //

    CryptoPP::BLAKE2b* blake2_hasher = new CryptoPP::BLAKE2b();

    void blake2_hash_text(const std::string& text, std::string& op);
    bool blake2_verify_hash(const std::string& text, const std::string& hash);

  public:
    EncryptionManager();
    ~EncryptionManager();

    void encrypt_text(const std::string& text, EncryptedMessage& op);
    void decrypt_text(const EncryptedMessage& msg, std::string& op);
};

}  // namespace phid

#endif /* ENCRYPTIONMANAGER_HPP */
