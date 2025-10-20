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
#include <zlc/gzipcomplete.hpp>

#include "phid/encryption/MessageTypes.hpp"
#include "utils.hpp"

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

    std::array<unsigned char, crypto_aead_chacha20poly1305_KEYBYTES> chacha_key;
    uint8_t chacha_num_uses = 0;  // same key can be used multiple times, my limit is 3

    /***/

    CryptoPP::BLAKE2b* blake2_hasher;

    /***/

    CryptoPP::RSA::PublicKey _public_key;
    CryptoPP::RSA::PrivateKey _private_key;

    /*****      *****\
    \*****      *****/


    //====================\\


    /*****  *****\
     HELPER FUNCS
    \*****  *****/

    /* op = OUTPUT, void bc more space efficient to output via ref */

    void compressText(const std::string& text, std::string& op);

    void decompressText(const std::string& text, std::string& op);

    /***/

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

    /***/

    void rsaEncryptChachaKey(const std::string& rsa_pub_key, std::string& op);

    void rsaDecryptChachaKey(const std::string& encrypted_key,
                             std::array<unsigned char, crypto_aead_chacha20poly1305_KEYBYTES>& op);

    /***/

    void blake2HashText(const std::string& text, std::string& op);
    bool blake2VerifyHash(const std::string& text, const std::string& hash);

    /*****  *****\
    \*****  *****/



  public:
    std::string rsa_priv_key;

    /** CONSTRUCTOR & DESTRUCTOR **/
    EncryptionManager(const std::string& rsa_priv_key = (std::string) "");
    ~EncryptionManager();  // all pointers deleted here
    /** **/

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

    void encryptText(const std::string& text, const std::string& rsa_pub_key, EncryptedMessage& op,
                     const int& version = 1);
    int decryptText(const EncryptedMessage& msg, std::string& op_text);
};

}  // namespace phid

#endif /* ENCRYPTIONMANAGER_HPP */
