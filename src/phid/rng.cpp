/*

 PHI
 2025/10/09

 Phi C++ Project
 src/phid/rng.c

 Zevi Berlin

*/

#include "phid/encryption/rng.hpp"

extern "C" {
int random_byte_gen(char* buf, const ssize_t len) {
  /*
  Fills a byte array with random bytes
  using /dev/urandom

  Args:
  buf - a char* that will be filled with random bytes
  len - the length of the char*

  Returns:
  0 if success, -1 if failure
  */

  int fd = open("/dev/urandom", O_RDONLY);
  if (fd == -1) {
    return -1;
  }

  ssize_t read_bytes = read(fd, buf, len);
  close(fd);

  if (read_bytes != len) {
    return -1;
  }

  return 0;
}
}