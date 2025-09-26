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

#include <zlc/gzipcomplete.hpp>
#include <cryptopp/cryptlib.h>
#include <cryptopp/blake2.h>

#include "phid/encryption/EncryptedMessage.hpp"

phid::EncryptionManager::EncryptionManager() {
  this->chacha_key.reserve(32);  // ChaCha20-Poly1305 keys are 32 bytes long
  this->num_uses = 0;
}

phid::EncryptionManager::~EncryptionManager() {
  this->compressor->finish();

  delete this->compressor;
  delete this->decompressor;
}

//

void phid::EncryptionManager::compress_text(const std::string& text,
                                            std::string& op) {
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

  this->compressor->finish();
}

void phid::EncryptionManager::decompress_text(const std::string& text,
                                              std::string& op) {
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

void phid::EncryptionManager::chacha_encrypt_text(const std::string& text,
                                                  int& op_nonce,
                                                  std::string& op_text) {
}

void phid::EncryptionManager::chacha_decrypt_text(
  const std::string& text, const std::string& decrypt_key, const int& nonce,
  std::string& op_text) {
}

//

void phid::EncryptionManager::blake2_hash_text(const std::string& text,
                                               std::string& op) {
  /* the following code is more or less copied from the examples here:
   http://www.cryptopp.com/wiki/BLAKE2 */

  this->blake2_hasher->Update((const byte*)text.data(), text.size());

  op.resize(this->blake2_hasher->DigestSize());

  this->blake2_hasher->Final((byte*)&op[0]);

  this->blake2_hasher->Restart();
}

bool phid::EncryptionManager::blake2_verify_hash(const std::string& text,
                                                 const std::string& hash) {
  std::string hashed_text;
  this->blake2_hash_text(text, hashed_text);

  if (hashed_text == hash) {
    return true;
  }

  return false;
}

/* public methods */

void phid::EncryptionManager::encrypt_text(const std::string& text,
                                           EncryptedMessage& op) {
  this->compress_text(text, op.content);
}

void phid::EncryptionManager::decrypt_text(const EncryptedMessage& msg,
                                           std::string& op) {
  this->decompress_text(msg.content, op);
}
