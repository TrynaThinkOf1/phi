/*

 PHI
 2025/09/24

 Phi C++ Project
 include/phi/encryption/Encryptor.hpp

 Zevi Berlin

*/

#ifndef ENCRYPTOR_HPP
#define ENCRYPTOR_HPP

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <cryptopp/blake2.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/osrng.h>
#include <cryptopp/queue.h>
#include <cryptopp/rsa.h>
#include <sodium.h>

#include "phi/encryption/MessageTypes.hpp"
#include "phi/encryption/gzip_basic.hpp"

namespace phi::encryption {

class Encryptor {
  private:
    CryptoPP::AutoSeededRandomPool* rng;

    /**/

    std::array<unsigned char, crypto_aead_chacha20poly1305_KEYBYTES> chacha_key;
    uint8_t chacha_num_uses = 0;  // same key can be used multiple times, my limit is 3

    /**/

    CryptoPP::BLAKE2b* blake2_hasher;

    /**/

    CryptoPP::RSA::PublicKey _public_key;
    CryptoPP::RSA::PrivateKey _private_key;

    /**/

    /* op = OUTPUT, void bc more space efficient to output via ref */

    void chachaEncryptText(
      const std::string& text,
      std::array<unsigned char, crypto_aead_chacha20poly1305_NPUBBYTES>& op_nonce,
      std::string& op_text);

    // int bc the decrypt function from libsodium returns -1 if nonce fails
    int chachaDecryptText(
      const std::string& text,
      const std::array<unsigned char, crypto_aead_chacha20poly1305_NPUBBYTES>& nonce,
      const std::array<unsigned char, crypto_aead_chacha20poly1305_KEYBYTES>& chacha_key,
      std::string& op_text);

    /**/

    void rsaEncryptChachaKey(const std::string& rsa_pub_key, std::string& op);

    void rsaDecryptChachaKey(const std::string& encrypted_key,
                             std::array<unsigned char, crypto_aead_chacha20poly1305_KEYBYTES>& op);

    /**/

    void blake2HashText(const std::string& text, std::string& op);
    bool blake2VerifyHash(const std::string& text, const std::string& hash);


  public:
    Encryptor(const std::string& rsa_priv_key = std::string(""));
    ~Encryptor();  // all pointers deleted here

    void rsaGenPair(std::string& op_pub, std::string& op_priv);

    void changePrivKey(std::string& new_rsa_priv_key);

    /***/

    /** TEMPLATE FUNCTIONS **/

    template <typename KeyType>
    static void rsaToStr(const KeyType& key, std::string& op) {
      CryptoPP::ByteQueue bqu;

      key.Save(bqu);

      op.resize(bqu.CurrentSize());
      bqu.Get(reinterpret_cast<unsigned char*>(op.data()), op.size());
    }

    template <typename KeyType>
    static void strToRsa(const std::string& key, KeyType& op) {
      CryptoPP::ByteQueue bqu;

      bqu.Put(reinterpret_cast<const unsigned char*>(key.data()), key.size());
      bqu.MessageEnd();

      op.Load(bqu);
    }

    /** **/

    /***/

    void encryptMessage(const std::string& text, const std::string& rsa_pub_key,
                        EncryptedMessage& op, int version = 1);
    int decryptMessage(const EncryptedMessage& msg, std::string& op_text);
};

}  // namespace phi::encryption

#endif /* ENCRYPTOR_HPP */
