/*

 PHI
 2025/09/24

 Phi C++ Project
 src/phi/Encryptor.cpp

 Zevi Berlin

*/

#include "phi/encryption/Encryptor.hpp"


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

#include "phi/encryption/MessageTypes.hpp"
#include "utils.hpp"

#define BUFFER (const int)16384
#define RSA_KEY_SIZE (const unsigned int)4096

/** CONSTRUCTOR & DESTRUCTOR **/

phi::encryption::Encryptor::Encryptor(const std::string& rsa_priv_key)
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


  crypto_aead_chacha20poly1305_keygen(this->chacha_key.data());

  /***/

  if (!rsa_priv_key.empty()) {
    phi::encryption::Encryptor::strToRsa<CryptoPP::RSA::PrivateKey>(rsa_priv_key,
                                                                    this->_private_key);
  }
}

phi::encryption::Encryptor::~Encryptor() {
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

void phi::encryption::Encryptor::compressText(const std::string& text, std::string& op) {
  /*
  Uses GZip with compression level 3
   to compress the plaintext message
   so that it saves space when being
   transferred via socket.

  Args:
   text - the string that needs compressing
   op - the string where compressed text goes
  */

  if (text.length() < BUFFER) {
    op = this->compressor->compress(text);
  } else {
    std::vector<std::string> chunks;
    for (size_t i = 0; i <= text.length() / BUFFER; i++) {
      size_t start_index = i * BUFFER;
      size_t end_index = (i + 1) * BUFFER;
      end_index = std::min(end_index, text.length());
      chunks.push_back(text.substr(start_index, end_index));
    }

    for (const std::string& chunk : chunks) {
      op += this->compressor->compress(chunk);
    }
  }
}

void phi::encryption::Encryptor::decompressText(const std::string& text, std::string& op) {
  /*
  Uses GZip to decompress a
   message into plaintext.

  Args:
   text - the compressed string
   op - the string where decompressed text is stored
  */

  if (text.length() < BUFFER) {
    op = this->decompressor->decompress(text);
  } else {
    std::vector<std::string> chunks;
    for (size_t i = 0; i <= text.length() / BUFFER; i++) {
      size_t start_index = i * BUFFER;
      size_t end_index = (i + 1) * BUFFER;
      end_index = std::min(end_index, text.length());
      chunks.push_back(text.substr(start_index, end_index));
    }

    for (const std::string& chunk : chunks) {
      op += this->decompressor->decompress(chunk);
    }
  }
}

/***/

void phi::encryption::Encryptor::chachaEncryptText(
  const std::string& text,
  std::array<unsigned char, crypto_aead_chacha20poly1305_NPUBBYTES>& op_nonce,
  std::string& op_text) {
  /*
  Uses the original ChaCha20-Poly1305 construction
   provided by libsodium to encrypt text
   https://doc.libsodium.org/secret-key_cryptography/aead/chacha20-poly1305

  Args:
   text - string containing the plaintext to encrypt
   op_nonce - a char array where the generated nonce will be stored in
   op_text - a string where the encrypted text will be stored
  */

  std::array<unsigned char, crypto_aead_chacha20poly1305_NPUBBYTES> nonce{};
  unsigned long long outlen = text.length() + crypto_aead_chacha20poly1305_ABYTES;
  std::vector<unsigned char> cipher(outlen);  // ciphertext

  /**/

  if (this->chacha_num_uses > 3) {
    crypto_aead_chacha20poly1305_keygen(this->chacha_key.data());
    this->chacha_num_uses = 0;
  }

  /**/

  randombytes_buf(nonce.data(), nonce.size());

  crypto_aead_chacha20poly1305_encrypt(
    cipher.data(),                                         // ciphertext output
    &outlen,                                               // ciphertext output length
    reinterpret_cast<const unsigned char*>(text.data()),   // plaintext input
    text.size(),                                           // plaintext length
    nullptr,                                               // additional data, not needed here
    0,                                                     // AD lengeth
    nullptr,                                               // "nsec always NULL" ~ libsodium docs
    reinterpret_cast<const unsigned char*>(nonce.data()),  // cryptosign nonce
    reinterpret_cast<const unsigned char*>(this->chacha_key.data())  // encryption/decryption key
  );

  /**/

  op_nonce.swap(nonce);
  op_text.assign(reinterpret_cast<const char*>(cipher.data()),
                 static_cast<size_t>(outlen));  // fuckass C++ typing
}

int phi::encryption::Encryptor::chachaDecryptText(
  const std::string& text,
  const std::array<unsigned char, crypto_aead_chacha20poly1305_NPUBBYTES>& nonce,
  const std::array<unsigned char, crypto_aead_chacha20poly1305_KEYBYTES>& chacha_key,
  std::string& op_text) {
  /*
  Uses the original ChaCha20-Poly1305 construction
   provided by libsodium to decrypt text
   https://doc.libsodium.org/secret-key_cryptography/aead/chacha20-poly1305

  Args:
   text - string containing the cipher to decrypt
   nonce - a char array with the generated nonce in it
   chacha_key - a char array containing the ChaCha20-Poly1305 decryption key
   op_text - string where the output decrypted text will be stored

  Returns:
   decryption fails ? -1 : 0
  */

  unsigned long long msglen = text.size() - crypto_aead_chacha20poly1305_ABYTES;
  std::vector<unsigned char> msg(msglen);

  /**/

  int success = crypto_aead_chacha20poly1305_decrypt(
    msg.data(),                                            // message output
    &msglen,                                               // message output length
    nullptr,                                               // "nsec always NULL" ~ libsodium docs
    reinterpret_cast<const unsigned char*>(text.data()),   // ciphertext input
    text.size(),                                           // ciphertext length
    nullptr,                                               // additional data, not needed here
    0,                                                     // AD length
    reinterpret_cast<const unsigned char*>(nonce.data()),  // cryptosign nonce
    reinterpret_cast<const unsigned char*>(this->chacha_key.data())  // encryption/decryption key
  );

  if (success == -1) {  // checks for tampering of message / wrong key
    return -1;
  }

  /**/

  op_text.assign(reinterpret_cast<const char*>(msg.data()),
                 static_cast<size_t>(msglen));  // fuckass C++ typing system
  return 0;
}

/***/

void phi::encryption::Encryptor::rsaEncryptChachaKey(const std::string& rsa_pub_key,
                                                     std::string& op) {
  /*
  Uses the Crypto++ implementation
   of RSA to encrypt a ChaCha20-Poly1305
   key. This is the hybrid encryption
   style of PGP.

  Args:
   chacha_key - char array that contains the ChaCha20-Poly1305 key
   rsa_pub_key - an RSA public key (which is used for encrypting the chacha key)
   op - string which will store the encrypted chacha key
  */

  CryptoPP::RSA::PublicKey pub;
  phi::encryption::Encryptor::strToRsa<CryptoPP::RSA::PublicKey>(rsa_pub_key, pub);

  /**/

  CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(pub);

  CryptoPP::StringSource(
    reinterpret_cast<const char*>(this->chacha_key.data()), true,
    new CryptoPP::PK_EncryptorFilter(*(this->rng), encryptor, new CryptoPP::StringSink(op)));
}

void phi::encryption::Encryptor::rsaDecryptChachaKey(
  const std::string& encrypted_key,
  std::array<unsigned char, crypto_aead_chacha20poly1305_KEYBYTES>& op) {
  /*
  Uses the Crypto++ implementation
   of RSA to decrypt an encrypted
   ChaCha20-Poly1305 key.

  Args:
   encrypted_key - string that contains the encrypted ChaCha20-Poly1305 key
   rsa_priv_key - an RSA private key (which is used for decrypting the chacha key)
   op - a char array which will be filled with the chacha key
  */

  std::string decoded;

  CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(this->_private_key);

  CryptoPP::StringSource strsrc(
    reinterpret_cast<const unsigned char*>(
      encrypted_key.data()),  // the c string version of the encrypted key
    encrypted_key.length(),   // explicit length to copy so that it doesnt stop at first \0
    true,                     // pump all
    new CryptoPP::PK_DecryptorFilter(*(this->rng), decryptor, new CryptoPP::StringSink(decoded)));

  std::memcpy(op.data(), decoded.data(), crypto_aead_chacha20poly1305_KEYBYTES);
}

/***/

void phi::encryption::Encryptor::blake2HashText(const std::string& text, std::string& op) {
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

  this->blake2_hasher->Update(reinterpret_cast<const unsigned char*>(text.data()), text.size());

  op.resize(this->blake2_hasher->DigestSize());  // save resize overhead early

  this->blake2_hasher->Final(reinterpret_cast<unsigned char*>(op.data()));

  this->blake2_hasher->Restart();  // refreshes the byte cache for new plaintext
}

bool phi::encryption::Encryptor::blake2VerifyHash(const std::string& text,
                                                  const std::string& hash) {
  /*
  Just uses the function above
   (phid::Encryptor::blake2_hash_text)
   to hash the text and then compares
   it to the provided hash
  */

  std::string hashed_text;
  this->blake2HashText(text, hashed_text);

  return hashed_text == hash;
}

/*****  *****\
\*****  *****/

//====================\\

/** public methods **/

void phi::encryption::Encryptor::rsaGenPair(std::string& op_pub, std::string& op_priv) {
  /*
  Generate a public/private key
   pair using Crypto++ RSA implementation

  Args:
   op_pub - string for the public key to be stored in
   op_priv - string for the private key to be stored in
  */

  CryptoPP::RSA::PrivateKey priv;
  CryptoPP::RSA::PublicKey pub;

  priv.GenerateRandomWithKeySize(*(this->rng), RSA_KEY_SIZE);
  pub = CryptoPP::RSA::PublicKey(priv);

  /**/

  phi::encryption::Encryptor::rsaToStr<CryptoPP::RSA::PublicKey>(pub, op_pub);
  phi::encryption::Encryptor::rsaToStr<CryptoPP::RSA::PublicKey>(priv, op_priv);
}

void phi::encryption::Encryptor::changePrivKey(std::string& new_rsa_priv_key) {
  phi::encryption::Encryptor::strToRsa<CryptoPP::RSA::PrivateKey>(new_rsa_priv_key,
                                                                  this->_private_key);
}

/***/

void phi::encryption::Encryptor::encryptMessage(const std::string& text,
                                                const std::string& rsa_pub_key,
                                                phi::encryption::EncryptedMessage& op,
                                                int version) {
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
  this->compressText(text, temp);
  this->chachaEncryptText(temp, op.nonce, op.content);

  this->rsaEncryptChachaKey(rsa_pub_key, op.chacha_key);
  this->blake2HashText(text, op.blake2_hash);
}

int phi::encryption::Encryptor::decryptMessage(const phi::encryption::EncryptedMessage& msg,
                                               std::string& op_text) {
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

  int success = 0;
  std::string temp;
  std::array<unsigned char, crypto_aead_chacha20poly1305_KEYBYTES> key{};

  switch (msg.version) {
    case 1:
      this->rsaDecryptChachaKey(msg.chacha_key, key);

      success = this->chachaDecryptText(msg.content, msg.nonce, key, temp);
      if (success == -1) {
        return -1;
      }

      this->decompressText(temp, op_text);

      if (!this->blake2VerifyHash(op_text, msg.blake2_hash)) {
        return -2;
      }

      return 0;
    default:
      return -3;
  }
}

/** **/
