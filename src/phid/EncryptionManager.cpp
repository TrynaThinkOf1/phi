/*

 PHI
 2025/09/24

 Phi C++ Project
 src/phid/EncryptionManager.cpp

 Zevi Berlin

*/

#include "phid/encryption//EncryptionManager.hpp"

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>

#include <zlc/gzipcomplete.hpp>
#include <cryptopp/cryptlib.h>
#include <cryptopp/blake2.h>
#include <cryptopp/rsa.h>
#include <cryptopp/queue.h>
#include <sodium.h>

#include "phid/encryption/EncryptedMessage.hpp"

phid::EncryptionManager::EncryptionManager() {
  //
  this->chacha_num_uses = 0;

  crypto_aead_chacha20poly1305_keygen(this->chacha_key);
}

phid::EncryptionManager::~EncryptionManager() {
  this->compressor->finish();

  delete this->compressor;
  delete this->decompressor;
}

//

void phid::EncryptionManager::compress_text(const std::string& text, std::string& op) {
  // 16384 is the size limit for byte stream to GZip
  if (text.length() < 16384) {
    op = this->compressor->compress(text);
  } else {
    std::vector<std::string> chunks;
    for (size_t i = 0; i <= text.length() / 16384; i++) {
      size_t si = i * 16384;
      size_t ei = (i + 1) * 16384;
      if (ei > text.length()) {
        ei = text.length();
      }
      chunks.push_back(text.substr(si, ei));
    }

    for (const std::string& c : chunks) {
      op += this->compressor->compress(c);
    }
  }
}

void phid::EncryptionManager::decompress_text(const std::string& text, std::string& op) {
  // 16384 is the size limit for byte stream to GZip
  if (text.length() < 16384) {
    op = this->decompressor->decompress(text);
  } else {
    std::vector<std::string> chunks;
    for (size_t i = 0; i <= text.length() / 16384; i++) {
      size_t si = i * 16384;
      size_t ei = (i + 1) * 16384;
      if (ei > text.length()) {
        ei = text.length();
      }
      chunks.push_back(text.substr(si, ei));
    }

    for (const std::string& c : chunks) {
      op += this->decompressor->decompress(c);
    }
  }
}

//

void phid::EncryptionManager::chacha_encrypt_text(
  const std::string& text, unsigned char (&op_nonce)[crypto_aead_chacha20poly1305_NPUBBYTES],
  std::string& op_text) {
  //
  unsigned char nonce[crypto_aead_chacha20poly1305_NPUBBYTES];
  unsigned long long outlen = text.length() + crypto_aead_chacha20poly1305_ABYTES;
  std::vector<unsigned char> ct(outlen);  // ciphertext

  if (this->chacha_num_uses > 3) {
    crypto_aead_chacha20poly1305_keygen(this->chacha_key);
    this->chacha_num_uses = 0;
  }

  randombytes_buf(nonce, sizeof nonce);
  crypto_aead_chacha20poly1305_encrypt(
    ct.data(),                                            // ciphertext output
    &outlen,                                              // ciphertext output length
    reinterpret_cast<const unsigned char*>(text.data()),  // plaintext input
    text.size(),                                          // plaintext length
    nullptr, 0,                                           // "additional data + length" -- idek
    NULL,   // "nsec is not used by this construction and should always be NULL" ~ libsodium docs
    nonce,  // cryptosign nonce
    this->chacha_key  // encryption/decryption key
  );

  std::memcpy(op_nonce, nonce, sizeof op_nonce);
  op_text.assign(reinterpret_cast<const char*>(ct.data()),
                 static_cast<size_t>(outlen));  // fuckass C++ typing system
}

int phid::EncryptionManager::chacha_decrypt_text(
  const std::string& text, const unsigned char (&nonce)[crypto_aead_chacha20poly1305_NPUBBYTES],
  const unsigned char (&chacha_key)[crypto_aead_chacha20poly1305_KEYBYTES], std::string& op_text) {
  //
  unsigned long long msglen = text.size() - crypto_aead_chacha20poly1305_ABYTES;
  std::vector<unsigned char> msg(msglen);

  int s = crypto_aead_chacha20poly1305_decrypt(
    msg.data(),  // message output
    &msglen,     // message output length
    NULL,  // "nsec is not used by this construction and should always be NULL" ~ libsodium docs
    reinterpret_cast<const unsigned char*>(text.data()),  // ciphertext input
    text.size(),                                          // ciphertext length
    nullptr, 0,  // "additional data + length" -- not used by encrypt function
    nonce,       // cryptosign nonce
    chacha_key);

  if (s == -1) {
    return -1;  // checks for tampering of message / wrong key
  }

  op_text.assign(reinterpret_cast<const char*>(msg.data()),
                 static_cast<size_t>(msglen));  // fuckass C++ typing system
  return 0;
}

//

void phid::EncryptionManager::rsa_encrypt_key(
  const unsigned char (&key)[crypto_aead_chacha20poly1305_KEYBYTES], const std::string& pub_key,
  std::string& op) {
}

void phid::EncryptionManager::rsa_decrypt_key(
  const std::string& key, const std::string& priv_key,
  unsigned char (&op)[crypto_aead_chacha20poly1305_KEYBYTES]) {
}

//

void phid::EncryptionManager::blake2_hash_text(const std::string& text, std::string& op) {
  /* the following code is more or less copied from the examples here:
   http://www.cryptopp.com/wiki/BLAKE2 */

  this->blake2_hasher->Update((const byte*)text.data(), text.size());

  op.resize(this->blake2_hasher->DigestSize());

  this->blake2_hasher->Final((byte*)&op[0]);

  this->blake2_hasher->Restart();
}

bool phid::EncryptionManager::blake2_verify_hash(const std::string& text, const std::string& hash) {
  //
  std::string hashed_text;
  this->blake2_hash_text(text, hashed_text);

  if (hashed_text == hash) {
    return true;
  }

  return false;
}

/* public methods */

void phid::EncryptionManager::gen_rsa_pair(std::string& op_priv, std::string& op_pub) {
  CryptoPP::RSA::PrivateKey priv;
  CryptoPP::RSA::PublicKey pub;

  priv.GenerateRandomWithKeySize(this->rng, 4096);
  pub = CryptoPP::RSA::PublicKey(priv);

  CryptoPP::ByteQueue q;

  priv.Save(q);
  op_priv.resize(q.CurrentSize());
  q.Get(reinterpret_cast<byte*>(&op_priv[0]), op_priv.size());
  q.Clear();

  pub.Save(q);
  op_pub.resize(q.CurrentSize());
  q.Get(reinterpret_cast<byte*>(&op_pub[0]), op_pub.size());
}

//

void phid::EncryptionManager::encrypt_text(const std::string& text, const std::string& rsa_pub_key,
                                           EncryptedMessage& op) {
  //
  std::string temp;
  this->compress_text(text, temp);
  this->chacha_encrypt_text(temp, op.nonce, op.content);
  std::memcpy(op.chacha_key, this->chacha_key, sizeof op.chacha_key);

  op.version = 1;
  // this->rsa_encrypt_text(this->chacha_key, rsa_pub_key, op.chacha_key);
  this->blake2_hash_text(text, op.blake2_hash);
}

int phid::EncryptionManager::decrypt_text(const EncryptedMessage& msg,
                                          const std::string& rsa_priv_key, std::string& op_text) {
  //
  int s;
  std::string temp;
  unsigned char key[crypto_aead_chacha20poly1305_KEYBYTES];

  switch (msg.version) {
    case 1:
      // s = this->chacha_decrypt_text(msg.content, msg.nonce, msg.chacha_key, temp);
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
