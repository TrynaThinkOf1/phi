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
#include <sodium.h>

#include "phid/encryption/EncryptedMessage.hpp"

typedef uint8_t byte;

namespace phid {

class EncryptionManager {
  private:
    zlibcomplete::GZipCompressor* compressor =
      new zlibcomplete::GZipCompressor(3, zlibcomplete::auto_flush);
    zlibcomplete::GZipDecompressor* decompressor = new zlibcomplete::GZipDecompressor();

    void compress_text(const std::string& text, std::string& op);
    void decompress_text(const std::string& text, std::string& op);

    //

    unsigned char chacha_key[crypto_aead_chacha20poly1305_KEYBYTES];
    uint8_t chacha_num_uses;

    void chacha_encrypt_text(const std::string& text,
                             unsigned char (&op_nonce)[crypto_aead_chacha20poly1305_NPUBBYTES],
                             std::string& op_text);

    int chacha_decrypt_text(
      const std::string& text, const unsigned char (&nonce)[crypto_aead_chacha20poly1305_NPUBBYTES],
      const unsigned char (&chacha_key)[crypto_aead_chacha20poly1305_KEYBYTES],
      std::string& op_text);

    //

    void rsa_encrypt_key(const unsigned char (&key)[crypto_aead_chacha20poly1305_KEYBYTES],
                         const std::string& pub_key, std::string& op);
    void rsa_decrypt_key(const std::string& key, const std::string& priv_key,
                         unsigned char (&op)[crypto_aead_chacha20poly1305_KEYBYTES]);

    //

    CryptoPP::BLAKE2b* blake2_hasher = new CryptoPP::BLAKE2b();

    void blake2_hash_text(const std::string& text, std::string& op);
    bool blake2_verify_hash(const std::string& text, const std::string& hash);

  public:
    EncryptionManager();
    ~EncryptionManager();

    void encrypt_text(const std::string& text, const std::string& rsa_pub_key,
                      EncryptedMessage& op);
    int decrypt_text(const EncryptedMessage& msg, const std::string& rsa_priv_key,
                     std::string& op_text);
};

}  // namespace phid

#endif /* ENCRYPTIONMANAGER_HPP */
