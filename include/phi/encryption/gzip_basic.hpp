/*

 PHI
 2025/12/21

 Phi C++ Project
 include/phi/encryption/gzip_basic.hpp

 Zevi Berlin

*/

#ifndef GZIP_BASIC_HPP
#define GZIP_BASIC_HPP

#include <string>
#include <stdexcept>

#include <zlib.h>

namespace phi::encryption {

static std::string gzipCompress(const std::string& input, int level = 3) {
  z_stream zstr{};
  zstr.zalloc = Z_NULL;
  zstr.zfree = Z_NULL;
  zstr.opaque = Z_NULL;

  if (deflateInit2(&zstr, level, Z_DEFLATED,
                   15 + 16,  // gzip
                   8, Z_DEFAULT_STRATEGY) != Z_OK) {
    throw std::runtime_error("deflateInit2 failed");
  }

  zstr.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(input.data()));
  zstr.avail_in = static_cast<uInt>(input.size());

  std::string output;
  char buffer[16384];

  int ret;
  do {
    zstr.next_out = reinterpret_cast<Bytef*>(buffer);
    zstr.avail_out = sizeof(buffer);

    ret = deflate(&zstr, Z_FINISH);
    output.append(buffer, sizeof(buffer) - zstr.avail_out);
  } while (ret == Z_OK);

  deflateEnd(&zstr);

  if (ret != Z_STREAM_END) {
    throw std::runtime_error("deflate failed");
  }

  return output;
}

/***/

static std::string gzipDecompress(const std::string& input) {
  z_stream zstr{};
  zstr.zalloc = Z_NULL;
  zstr.zfree = Z_NULL;
  zstr.opaque = Z_NULL;

  if (inflateInit2(&zstr, 15 + 16) != Z_OK) {
    throw std::runtime_error("inflateInit2 failed");
  }

  zstr.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(input.data()));
  zstr.avail_in = static_cast<uInt>(input.size());

  std::string output;
  char buffer[16384];

  int ret;
  do {
    zstr.next_out = reinterpret_cast<Bytef*>(buffer);
    zstr.avail_out = sizeof(buffer);

    ret = inflate(&zstr, 0);
    output.append(buffer, sizeof(buffer) - zstr.avail_out);
  } while (ret == Z_OK);

  inflateEnd(&zstr);

  if (ret != Z_STREAM_END) {
    throw std::runtime_error("inflate failed");
  }

  return output;
}

}  // namespace phi::encryption

#endif /* GZIP_BASIC_HPP */