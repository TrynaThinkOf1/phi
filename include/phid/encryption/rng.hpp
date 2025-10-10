/*

 PHI
 2025/10/09

 Phi C++ Project
 include/phid/encryption/rng.h

 Zevi Berlin

*/

#ifndef RNG_HPP
#define RNG_HPP

extern "C" {

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int random_byte_gen(char* buf, const ssize_t len);
}

#endif /* RNG_HPP */
