/*

 PHI
 2025/10/10

 Phi C++ Project
 include/phid/encryption/secrets.hpp

 Zevi Berlin

*/

#ifndef SECRETS_HPP
#define SECRETS_HPP

#include <iostream>
#include <string>
#include <vector>

#include <sodium.h>

#include "phid/encryption/MessageTypes.hpp"

namespace phid {

void generate_shared_secret(std::vector<unsigned char>& op_secret);

/***/

void add_mac_to_msg(const std::vector<unsigned char>& key, AuthenticatedUpdate& msg);

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

  if (key.size() != 32) {
    // TODO: Fatal error handling for daemon
    std::cout << "Secret key is of wrong size: " << key.size() << std::endl;
    return;
  }

  msg.mac.resize(32);
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
      if (crypto_auth_verify(reinterpret_cast<const unsigned char*>(msg.mac.data()),
                             reinterpret_cast<const unsigned char*>(msg.content.data()),
                             msg.content.size(),
                             reinterpret_cast<const unsigned char*>(key.data())) != 0) {
        return false;
      } else {
        return true;
      }

    default:
      return false;
  }
}

/** **/

/**/

bool verify_mac_msg(const std::vector<unsigned char>& key, const AuthenticatedUpdate& msg);

bool verify_mac_msg(const std::vector<unsigned char>& key, const EncryptedMessage& msg);

/***/

std::vector<unsigned char> str_to_secret(const std::string& secret);

std::string secret_to_str(const std::vector<unsigned char>& secret);

}  // namespace phid

#endif /* SECRETS_HPP */
