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

void phid::generate_kx_pair(phid::kxp& op) {
  /*
  Generate a private and a public key
   for Diffie-Hellmann key exchange

  Args:
   op_pk - output key pair struct
  */

  crypto_kx_keypair(op.pk, op.sk);
}

/***/

bool phid::derive_shared_secret(const bool is_alice, const phid::kxp& self,
                                const unsigned char (&peer_pk)[crypto_kx_PUBLICKEYBYTES],
                                std::vector<unsigned char>& op_key) {
  /*
  Derive using the Diffie-Hellmann algorithm
   a shared secret between Alice and Bob

  Args:
   is_client - important self explanatory
   self - my key exchange pair struct
   peer_pk - my peer's public key
   op_key - vector shared secret
  */

  op_key.resize(crypto_kx_SESSIONKEYBYTES);

  int rc;
  if (is_alice) {
    rc = crypto_kx_client_session_keys(nullptr,        // receive key data, not used
                                       op_key.data(),  // transmit key, the shared secret
                                       self.pk,        // my public key
                                       self.sk,        // my private key
                                       peer_pk         // my peer's public key
    );
  } else {
    rc = crypto_kx_server_session_keys(nullptr,        // receive key data, not used
                                       op_key.data(),  // transmit key, the shared secret
                                       self.pk,        // my public key
                                       self.sk,        // my private key
                                       peer_pk         // my peer's public key
    );
  }

  if (rc != 0) return false;
  return true;
}