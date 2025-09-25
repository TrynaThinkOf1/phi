/*

 PHI
 2025/09/24

 Phi C++ Project
 src/phid/EncryptionManager.cpp

 Zevi Berlin

*/

#include "phid/encryption//EncryptionManager.hpp"

#include <string>
#include <cstdint>

#include <zlc/gzipcomplete.hpp>

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
  op = this->compressor->compress(text);
}

void phid::EncryptionManager::decompress_text(const std::string& text,
                                              std::string& op) {
  op = this->decompressor->decompress(text);
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
