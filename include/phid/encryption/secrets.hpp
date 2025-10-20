/*

 PHI
 2025/10/10

 Phi C++ Project
 include/phid/encryption/secrets.hpp

 Zevi Berlin

*/

#ifndef SECRETS_HPP
#define SECRETS_HPP

#include <array>
#include <iostream>
#include <string>
#include <vector>

#include <sodium.h>

#include "phid/encryption/MessageTypes.hpp"

namespace phid {

// key exchange pair
struct KXP {
    std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> pk;
    std::array<unsigned char, crypto_kx_SECRETKEYBYTES> sk;
};

/***/

void genKXP(KXP& op);

/***/

bool derive_shared_secret(const bool& is_alice, const phid::KXP& self,
                          const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES>& peer_pk,
                          std::vector<unsigned char>& op_key);

/** TEMPLATE FUNCTIONS **/

template <typename T>
void add_mac_to_msg(const std::vector<unsigned char>& key, T& msg) {
  /*
  Add a MAC to an update/message to
   make it authenticated

  Args:
   key - the vector shared secret
   msg - update/message to add MAC to
  */

  if (key.size() != crypto_auth_KEYBYTES) {
    // TODO: Fatal error handling for daemon
    std::cout << "Secret key is of wrong size: " << key.size() << "\n";
    return;
  }

  msg.mac.resize(crypto_auth_BYTES);
  crypto_auth(reinterpret_cast<unsigned char*>(msg.mac.data()),
              reinterpret_cast<const unsigned char*>(msg.content.data()), msg.content.size(),
              reinterpret_cast<const unsigned char*>(key.data()));
}

template <typename T>
bool verify_mac_in_msg(const std::vector<unsigned char>& key, T& msg) {
  /*
  Uses the MAC from the update/message
   to verify the integrity of the sender

  Args:
   key - shared secret key
   msg - update/message with content and a MAC

  Returns:
   true/false depending upon verification
  */

  switch (msg.version) {
    case 1:
      return crypto_auth_verify(reinterpret_cast<const unsigned char*>(msg.mac.data()),
                                reinterpret_cast<const unsigned char*>(msg.content.data()),
                                msg.content.size(),
                                reinterpret_cast<const unsigned char*>(key.data()));

    default:
      return false;
  }
}

/** **/

inline std::vector<unsigned char> str_to_secret(const std::string& secret) {
  return std::vector<unsigned char>(secret.begin(), secret.end());
}

inline std::string secret_to_str(const std::vector<unsigned char>& secret) {
  return std::string(secret.begin(), secret.end());
}

}  // namespace phid

#endif /* SECRETS_HPP */
