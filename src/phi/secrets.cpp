/*

 PHI
 2025/10/10

 Phi C++ Project
 src/phi/secrets.cpp

 Zevi Berlin

*/

#include "phi/encryption/secrets.hpp"

#include <iostream>
#include <string>
#include <vector>

#include <sodium.h>

#include "phi/encryption/MessageTypes.hpp"

void phi::encryption::genKXP(phi::encryption::KXP& op) {
  /*
  Generate a private and a public key
   for Diffie-Hellmann key exchange

  Args:
   op - output key pair struct
  */

  crypto_kx_keypair(op.pk.data(), op.sk.data());
}

/***/

bool phi::encryption::derive_shared_secret(
  const bool& is_alice, const phi::encryption::KXP& self,
  const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES>& peer_pk,
  std::vector<unsigned char>& op_key) {
  /*
  Derive using the Diffie-Hellmann algorithm
   a shared secret between Alice and Bob

  Args:
   is_client - important self explanatory
   self - my key exchange pair struct
   peer_pk - peer's public key
   op_key - vector shared secret
  */

  op_key.resize(crypto_kx_SESSIONKEYBYTES);

  int code = 0;
  if (is_alice) {
    code = crypto_kx_client_session_keys(
      nullptr,                                           // receive key data, not used
      op_key.data(),                                     // transmit key, the shared secret
      const_cast<const unsigned char*>(self.pk.data()),  // my public key
      const_cast<const unsigned char*>(self.sk.data()),  // my private key
      peer_pk.data()                                     // my peer's public key
    );
  } else {
    code = crypto_kx_server_session_keys(
      nullptr,                                           // receive key data, not used
      op_key.data(),                                     // transmit key, the shared secret
      const_cast<const unsigned char*>(self.pk.data()),  // my public key
      const_cast<const unsigned char*>(self.sk.data()),  // my private key
      peer_pk.data()                                     // my peer's public key
    );
  }

  if (code != 0) {
    return false;
  }
  return true;
}