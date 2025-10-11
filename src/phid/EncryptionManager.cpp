/*

 PHI
 2025/09/24

 Phi C++ Project
 src/phid/EncryptionManager.cpp

 Zevi Berlin

*/

#include "phid/encryption/EncryptionManager.hpp"


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

/** CONSTRUCTOR & DESTRUCTOR **/

phid::EncryptionManager::EncryptionManager(const std::string& rsa_priv_key)
    : compressor(new zlibcomplete::GZipCompressor(3)),
      decompressor(new zlibcomplete::GZipDecompressor()),
      rng(new CryptoPP::AutoSeededRandomPool()),
      blake2_hasher(new CryptoPP::BLAKE2b()) {
  /*
  Create the ChaCha20Poly1305 key,
   GZipCompressor/Decompressor, the
   BLAKE2b hasher, the ASRP, and the
   RSA private key.
  */


  crypto_aead_chacha20poly1305_keygen(this->chacha_key);

  /***/

  if (!rsa_priv_key.empty()) {
    this->rsa_priv_key = rsa_priv_key;
    phid::EncryptionManager::str_to_rsa<CryptoPP::RSA::PrivateKey>(rsa_priv_key, this->__priv);
  }
}

phid::EncryptionManager::~EncryptionManager() {
  /*
  Delete the ChaCha20Poly1305 key, and
   GZipCompressor/Decompressor, the
   BLAKE2b hasher, and the ASRP.
  */

  this->compressor->finish();

  delete this->compressor;
  delete this->decompressor;

  /***/

  delete this->rng;

  /***/

  delete this->blake2_hasher;
}

/** **/

//====================\\

/*****  *****\
 HELPER FUNCS
\*****  *****/

void phid::EncryptionManager::compress_text(const std::string& text, std::string& op) {
  /*
  Uses GZip with compression level 3
   to compress the plaintext message
   so that it saves space when being
   transferred via socket.

  Args:
   text - the string that needs compressing
   op - the string where compressed text goes
  */

  // 16384 is the size limit for byte stream to GZip
  if (text.length() < 16384) {
    op = this->compressor->compress(text);
  } else {
    std::vector<std::string> chunks;
    for (size_t i = 0; i <= text.length() / 16384; i++) {
      size_t si = i * 16384;
      size_t ei = (i + 1) * 16384;
      ei = std::min(ei, text.length());
      chunks.push_back(text.substr(si, ei));
    }

    for (const std::string& c : chunks) {
      op += this->compressor->compress(c);
    }
  }
}

void phid::EncryptionManager::decompress_text(const std::string& text, std::string& op) {
  /*
  Uses GZip to decompress a
   message into plaintext.

  Args:
   text - the compressed string
   op - the string where decompressed text is stored
  */

  // 16384 is the size limit for byte stream to GZip
  if (text.length() < 16384) {
    op = this->decompressor->decompress(text);
  } else {
    std::vector<std::string> chunks;
    for (size_t i = 0; i <= text.length() / 16384; i++) {
      size_t si = i * 16384;
      size_t ei = (i + 1) * 16384;
      ei = std::min(ei, text.length());
      chunks.push_back(text.substr(si, ei));
    }

    for (const std::string& c : chunks) {
      op += this->decompressor->decompress(c);
    }
  }
}

/***/

void phid::EncryptionManager::chacha_encrypt_text(
  const std::string& text, unsigned char (&op_nonce)[crypto_aead_chacha20poly1305_NPUBBYTES],
  std::string& op_text) {
  /*
  Uses the original ChaCha20-Poly1305 construction
   provided by libsodium to encrypt text
   https://doc.libsodium.org/secret-key_cryptography/aead/chacha20-poly1305

  Args:
   text - string containing the plaintext to encrypt
   op_nonce - a char* where the generated nonce will be stored in
   op_text - a string where the encrypted text will be stored
  */

  unsigned char nonce[crypto_aead_chacha20poly1305_NPUBBYTES];
  unsigned long long outlen = text.length() + crypto_aead_chacha20poly1305_ABYTES;
  std::vector<unsigned char> ct(outlen);  // ciphertext

  /**/

  if (this->chacha_num_uses > 3) {
    crypto_aead_chacha20poly1305_keygen(this->chacha_key);
    this->chacha_num_uses = 0;
  }

  /**/

  randombytes_buf(nonce, sizeof nonce);

  crypto_aead_chacha20poly1305_encrypt(
    ct.data(),                                            // ciphertext output
    &outlen,                                              // ciphertext output length
    reinterpret_cast<const unsigned char*>(text.data()),  // plaintext input
    text.size(),                                          // plaintext length
    nullptr,                                              // additional data, not needed here
    0,                                                    // AD lengeth
    nullptr,                                              // "nsec always NULL" ~ libsodium docs
    nonce,                                                // cryptosign nonce
    this->chacha_key                                      // encryption/decryption key
  );

  /**/

  std::memcpy(op_nonce, nonce, sizeof op_nonce);  // because nonce is an array
  op_text.assign(reinterpret_cast<const char*>(ct.data()),
                 static_cast<size_t>(outlen));  // fuckass C++ typing
}

int phid::EncryptionManager::chacha_decrypt_text(
  const std::string& text, const unsigned char (&nonce)[crypto_aead_chacha20poly1305_NPUBBYTES],
  const unsigned char (&chacha_key)[crypto_aead_chacha20poly1305_KEYBYTES], std::string& op_text) {
  /*
  Uses the original ChaCha20-Poly1305 construction
   provided by libsodium to decrypt text
   https://doc.libsodium.org/secret-key_cryptography/aead/chacha20-poly1305

  Args:
   text - string containing the cipher to decrypt
   nonce - a char* with the generated nonce in it
   chacha_key - a char* containing the ChaCha20-Poly1305 decryption key
   op_text - string where the output decrypted text will be stored

  Returns:
   encryption fails ? -1 : 0
  */

  unsigned long long msglen = text.size() - crypto_aead_chacha20poly1305_ABYTES;
  std::vector<unsigned char> msg(msglen);

  /**/

  int s = crypto_aead_chacha20poly1305_decrypt(
    msg.data(),                                           // message output
    &msglen,                                              // message output length
    nullptr,                                              // "nsec always NULL" ~ libsodium docs
    reinterpret_cast<const unsigned char*>(text.data()),  // ciphertext input
    text.size(),                                          // ciphertext length
    nullptr,                                              // additional data, not needed here
    0,                                                    // AD length
    nonce,                                                // cryptosign nonce
    chacha_key                                            // encryption/decryption key
  );

  if (s == -1) {  // checks for tampering of message / wrong key
    return -1;
  }

  /**/

  op_text.assign(reinterpret_cast<const char*>(msg.data()),
                 static_cast<size_t>(msglen));  // fuckass C++ typing system
  return 0;
}

/***/

void phid::EncryptionManager::rsa_encrypt_chacha_key(
  const unsigned char (&chacha_key)[crypto_aead_chacha20poly1305_KEYBYTES],
  const std::string& rsa_pub_key, std::string& op) {
  /*
  Uses the Crypto++ implementation
   of RSA to encrypt a ChaCha20-Poly1305
   key. This is the hybrid encryption
   style of PGP.

  Args:
   chacha_key - char* that contains the ChaCha20-Poly1305 key
   rsa_pub_key - an RSA public key (which is used for encrypting the chacha key)
   op - string which will store the encrypted chacha key
  */

  CryptoPP::RSA::PublicKey pub;
  phid::EncryptionManager::str_to_rsa<CryptoPP::RSA::PublicKey>(rsa_pub_key, pub);

  /**/

  CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(pub);

  CryptoPP::StringSource(
    reinterpret_cast<const char*>(chacha_key), true,
    new CryptoPP::PK_EncryptorFilter(*(this->rng), encryptor, new CryptoPP::StringSink(op)));
}

void phid::EncryptionManager::rsa_decrypt_chacha_key(
  const std::string& encrypted_key, unsigned char (&op)[crypto_aead_chacha20poly1305_KEYBYTES]) {
  /*
  Uses the Crypto++ implementation
   of RSA to decrypt an encrypted
   ChaCha20-Poly1305 key.

  Args:
   encrypted_key - string that contains the encrypted ChaCha20-Poly1305 key
   rsa_priv_key - an RSA private key (which is used for decrypting the chacha key)
   op - a char* which will be filled with the chacha key
  */

  std::string decoded;

  CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(this->__priv);

  CryptoPP::StringSource strsrc(
    reinterpret_cast<const unsigned char*>(
      encrypted_key.data()),  // the c string version of the encrypted key
    encrypted_key.length(),   // explicit length to copy so that it doesnt stop at first \0
    true,                     // pump all
    new CryptoPP::PK_DecryptorFilter(*(this->rng), decryptor, new CryptoPP::StringSink(decoded)));

  std::memcpy(op, decoded.data(), crypto_aead_chacha20poly1305_KEYBYTES);
}

/***/

void phid::EncryptionManager::blake2_hash_text(const std::string& text, std::string& op) {
  /*
  Uses the Crypto++ implementation of
   the BLAKE2b hashing algorithm. This
   will hash plaintext so that after
   decryption, the message can be verified
  (more or less copied from here: http://www.cryptopp.com/wiki/BLAKE2)

  Args:
   text - plaintext to hash
   op - where the new hash string will go
  */

  this->blake2_hasher->Update((const byte*)text.data(), text.size());

  op.resize(this->blake2_hasher->DigestSize());  // save resize overhead early

  this->blake2_hasher->Final((byte*)op.data());

  this->blake2_hasher->Restart();  // refreshes the byte cache for new plaintext
}

bool phid::EncryptionManager::blake2_verify_hash(const std::string& text, const std::string& hash) {
  /*
  Just uses the function above
   (phid::EncryptionManager::blake2_hash_text)
   to hash the text and then compares
   it to the provided hash
  */

  std::string hashed_text;
  this->blake2_hash_text(text, hashed_text);

  return hashed_text == hash;
}

/*****  *****\
\*****  *****/

//====================\\

/** public methods **/

void phid::EncryptionManager::gen_rsa_pair(std::string& op_pub, std::string& op_priv) {
  /*
  Generate a public/private key
   pair using Crypto++ RSA implementation

  Args:
   op_pub - string for the public key to be stored in
   op_priv - string for the private key to be stored in
  */

  CryptoPP::RSA::PrivateKey priv;
  CryptoPP::RSA::PublicKey pub;

  priv.GenerateRandomWithKeySize(*(this->rng), 4096);
  pub = CryptoPP::RSA::PublicKey(priv);

  /**/

  phid::EncryptionManager::rsa_to_str<CryptoPP::RSA::PublicKey>(pub, op_pub);
  phid::EncryptionManager::rsa_to_str<CryptoPP::RSA::PublicKey>(priv, op_priv);
}

void phid::EncryptionManager::change_rsa_priv_key(std::string& new_rsa_priv_key) {
  this->rsa_priv_key = new_rsa_priv_key;
  phid::EncryptionManager::str_to_rsa<CryptoPP::RSA::PrivateKey>(new_rsa_priv_key, this->__priv);
}

/***/

void phid::EncryptionManager::encrypt_text(const std::string& text, const std::string& rsa_pub_key,
                                           EncryptedMessage& op, int version) {
  /*
  Uses a PGP-style standard:
   GZip (level 3) for compression,
   ChaCha20-Poly1305 for encryption,
   RSA for encryption of the chacha
   key (for hybrid encryption),
   and BLAKE2b for hashing (for tamper
   verification)

  Args:
   text - string to compress, encrypt, and hash
   rsa_pub_key - public key of client to encrypt chacha key
   op - output struct containing message information
   version - *OPTIONAL* for setting a custom version for message
  */

  std::string temp;

  op.version = version;
  this->compress_text(text, temp);
  this->chacha_encrypt_text(temp, op.nonce, op.content);

  this->rsa_encrypt_chacha_key(this->chacha_key, rsa_pub_key, op.chacha_key);
  this->blake2_hash_text(text, op.blake2_hash);
}

int phid::EncryptionManager::decrypt_text(const EncryptedMessage& msg, std::string& op_text) {
  /*
  Reverses the process of encryption
   (see phid::EncryptionManager::encrypt_text).
   No RSA key necessary because its a class variable.

  Args:
   msg - struct containing message information
   op_text - string containing the plaintext message

  Returns:
   Any integer representing an error
  */

  int s = 0;
  std::string temp;
  unsigned char key[crypto_aead_chacha20poly1305_KEYBYTES];

  switch (msg.version) {
    case 1:
      this->rsa_decrypt_chacha_key(msg.chacha_key, key);

      s = this->chacha_decrypt_text(msg.content, msg.nonce, key, temp);
      if (s == -1) {
        return -1;
      }

      this->decompress_text(temp, op_text);

      if (!this->blake2_verify_hash(op_text, msg.blake2_hash)) {
        return -2;
      }

      return 0;
    default:
      return -3;
  }
}

/** **/
