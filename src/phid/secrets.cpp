/*

 PHI
 2025/10/10

 Phi C++ Project
 src/phid/secrets.cpp

 Zevi Berlin

*/

#include "phid/encryption/secrets.hpp"

#include <iostream>
#include <string>
#include <vector>

#include <sodium.h>

#include "phid/encryption/MessageTypes.hpp"

void phid::generate_shared_secret(std::vector<unsigned char>& op_secret) {
  /*
  Generate a 32 byte secret

  Args:
   op_secret - output char vector
  */

  op_secret.resize(32);
  crypto_auth_keygen(op_secret.data());
}

/***/

std::vector<unsigned char> str_to_secret(const std::string& secret) {
  /*
  Convert a std::string to a shared
   secret vector.
   Necessary because database holds strings
   while functions take vectors.

  Args:
   secret - string of secret from database

  Returns:
   vector version of secret
  */

  return std::vector<unsigned char>{secret.data(), secret.data() + secret.size()};
}

std::string secret_to_str(const std::vector<unsigned char>& secret) {
  /*
  Convert a std::vector to a shared
   secret string.
   Necessary because database holds strings
   while functions take vectors.

  Args:
   secret - vector secret

  Returns:
   string version of secret
  */

  return std::string(secret.begin(), secret.end());
}