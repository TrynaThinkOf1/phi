/*

 PHI
 2025/09/24

 Phi C++ Project
 include/phid/encryption/EncryptionManager.hpp

 Zevi Berlin

*/

#ifndef ENCRYPTIONMANAGER_HPP
#define ENCRYPTIONMANAGER_HPP

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <zlc/gzipcomplete.hpp>
#include <cryptopp/cryptlib.h>
#include <cryptopp/blake2.h>
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <cryptopp/queue.h>
#include <sodium.h>

#include "phid/encryption/MessageTypes.hpp"
#include "utils.hpp"

typedef uint8_t byte;

namespace phid {

class EncryptionManager {
  private:
    /*****      *****\
     LIFETIME OBJECTS
    \*****      *****/

    zlibcomplete::GZipCompressor* compressor;
    // 3 = compression level, 1-9 (least-greatest)
    zlibcomplete::GZipDecompressor* decompressor;

    /***/

    CryptoPP::AutoSeededRandomPool* rng;

    /***/

    unsigned char chacha_key[crypto_aead_chacha20poly1305_KEYBYTES]{};
    uint8_t chacha_num_uses = 0;  // same key can be used multiple times, my limit is 3

    /***/

    CryptoPP::BLAKE2b* blake2_hasher;

    /***/

    CryptoPP::RSA::PrivateKey __priv;

    /*****      *****\
    \*****      *****/


    //====================\\


    /*****  *****\
     HELPER FUNCS
    \*****  *****/

    /* op = OUTPUT, void bc more space efficient to output via ref */

    void compress_text(const std::string& text, std::string& op);

    void decompress_text(const std::string& text, std::string& op);

    /***/

    void chacha_encrypt_text(const std::string& text,
                             unsigned char (&op_nonce)[crypto_aead_chacha20poly1305_NPUBBYTES],
                             std::string& op_text);

    static int chacha_decrypt_text(
      const std::string& text, const unsigned char (&nonce)[crypto_aead_chacha20poly1305_NPUBBYTES],
      const unsigned char (&chacha_key)[crypto_aead_chacha20poly1305_KEYBYTES],
      std::string& op_text);
    // int bc the decrypt function from libsodium returns -1 if nonce fails

    /***/

    void rsa_encrypt_chacha_key(
      const unsigned char (&chacha_key)[crypto_aead_chacha20poly1305_KEYBYTES],
      const std::string& rsa_pub_key, std::string& op);

    void rsa_decrypt_chacha_key(const std::string& encrypted_key,
                                unsigned char (&op)[crypto_aead_chacha20poly1305_KEYBYTES]);

    /***/

    void blake2_hash_text(const std::string& text, std::string& op);
    bool blake2_verify_hash(const std::string& text, const std::string& hash);

    /*****  *****\
    \*****  *****/



  public:
    std::string rsa_priv_key;

    /** CONSTRUCTOR & DESTRUCTOR **/
    EncryptionManager(const std::string& rsa_priv_key = (std::string) "");
    ~EncryptionManager();  // all pointers deleted here
    /** **/

    void gen_rsa_pair(std::string& op_pub, std::string& op_priv);

    void change_rsa_priv_key(std::string& new_rsa_priv_key);

    /***/

    /** TEMPLATE FUNCTIONS **/

    template <typename T>
    static void rsa_to_str(const T& key, std::string& op) {
      CryptoPP::ByteQueue q;

      key.Save(q);

      op.resize(q.CurrentSize());
      q.Get(reinterpret_cast<byte*>(&op[0]), op.size());
    }

    template <typename T>
    static void str_to_rsa(const std::string& key, T& op) {
      CryptoPP::ByteQueue q;

      q.Put(reinterpret_cast<const byte*>(key.data()), key.size());
      q.MessageEnd();

      op.Load(q);
    }

    /** **/

    /***/

    void encrypt_text(const std::string& text, const std::string& rsa_pub_key, EncryptedMessage& op,
                      int version = 1);
    int decrypt_text(const EncryptedMessage& msg, std::string& op_text);
};

}  // namespace phid

#endif /* ENCRYPTIONMANAGER_HPP */
