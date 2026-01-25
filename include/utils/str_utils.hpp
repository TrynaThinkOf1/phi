#ifndef STR_UTILS_HPP
#define STR_UTILS_HPP


#include <iostream>
#include <string>
#include <cstdio>
#include <sstream>
#include <iomanip>

#include <termios.h>
#include <unistd.h>

#include <cryptopp/base64.h>

//---------> [ Config. Separator ] <---------\\ 

[[nodiscard]] static std::string getHiddenInput() {
  // CODE BELOW IS VIA STACK OVERFLOW {
  termios oldt{};
  tcgetattr(STDIN_FILENO, &oldt);
  termios newt = oldt;
  newt.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  // }
  std::string input;
  std::getline(std::cin, input);
  // CLEANUP {
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  // }

  return input;
}

//------------[ Func. Implementation Separator ]------------\\ 

[[nodiscard]] static std::string toB64(const std::string& str) {
  std::string opt;

  CryptoPP::Base64Encoder encoder(nullptr, false);
  encoder.Put(reinterpret_cast<const unsigned char*>(str.data()), str.size());
  encoder.MessageEnd();

  CryptoPP::word64 size = encoder.MaxRetrievable();
  if (size > 0) {
    opt.resize(size);
    encoder.Get(reinterpret_cast<unsigned char*>(opt.data()), size - 1);
  }

  return opt;
}

//------------[ Func. Implementation Separator ]------------\\ 

[[nodiscard]] static std::string fromB64(const std::string& str) {
  std::string opt;

  CryptoPP::Base64Decoder decoder(nullptr);
  decoder.Put(reinterpret_cast<const unsigned char*>(str.data()), str.size());
  decoder.MessageEnd();

  CryptoPP::word64 size = decoder.MaxRetrievable();
  if (size > 0) {
    opt.resize(size);
    decoder.Get(reinterpret_cast<unsigned char*>(opt.data()), size);
  }

  return opt;
}

//------------[ Func. Implementation Separator ]------------\\ 

[[nodiscard]] static int numEmojis(const std::string& str) {
  {
    auto nextCP = [&](size_t &idx) -> unsigned int {
      const size_t len = str.size();
      if (idx >= len) return 0;
      const unsigned char *s = reinterpret_cast<const unsigned char*>(str.data());
      unsigned char c = s[idx];
      if (c < 0x80) { idx += 1; return c; }
      if ((c >> 5) == 0x6) { if (idx + 1 >= len) { idx = len; return 0; } unsigned int cp = ((c & 0x1F) << 6) | (s[idx+1] & 0x3F); idx += 2; return cp; }
      if ((c >> 4) == 0xE) { if (idx + 2 >= len) { idx = len; return 0; } unsigned int cp = ((c & 0x0F) << 12) | ((s[idx+1] & 0x3F) << 6) | (s[idx+2] & 0x3F); idx += 3; return cp; }
      if ((c >> 3) == 0x1E) { if (idx + 3 >= len) { idx = len; return 0; } unsigned int cp = ((c & 0x07) << 18) | ((s[idx+1] & 0x3F) << 12) | ((s[idx+2] & 0x3F) << 6) | (s[idx+3] & 0x3F); idx += 4; return cp; }
      // invalid/unsupported byte; skip one
      idx += 1;
      return 0;
    };

    auto isRegional = [](unsigned int cp)->bool {
      return cp >= 0x1F1E6u && cp <= 0x1F1FFu;
    };
    auto isEmojiModifier = [](unsigned int cp)->bool {
      return cp >= 0x1F3FBu && cp <= 0x1F3FFu;
    };
    auto isEmojiBase = [](unsigned int cp)->bool {
      return
        (cp >= 0x1F600u && cp <= 0x1F64Fu) || // emoticons
        (cp >= 0x1F300u && cp <= 0x1F5FFu) || // symbols & pictographs
        (cp >= 0x1F680u && cp <= 0x1F6FFu) || // transport & map
        (cp >= 0x2600u  && cp <= 0x26FFu)  || // misc symbols
        (cp >= 0x2700u  && cp <= 0x27BFu)  || // dingbats
        (cp >= 0x1F900u && cp <= 0x1F9FFu) || // supplemental
        (cp >= 0x1FA70u && cp <= 0x1FAFFu) || // extended-A
        (cp == 0x00A9u) || (cp == 0x00AEu) || (cp == 0x203Cu) || (cp == 0x2049u) || (cp == 0x3030u) || (cp == 0x303Du);
    };

    const unsigned int ZWJ = 0x200Du;
    const unsigned int VS16 = 0xFE0Fu;
    const unsigned int KEYCAP = 0x20E3u;

    size_t i = 0;
    int count = 0;
    const size_t len = str.size();

    while (i < len) {
      size_t cur = i;
      unsigned int cp = nextCP(cur);
      if (cp == 0) { i = cur; continue; }

      // Regional indicator pair -> flag
      if (isRegional(cp)) {
        size_t peek = cur;
        unsigned int cp2 = nextCP(peek);
        if (cp2 != 0 && isRegional(cp2)) {
          count += 1;
          i = peek;
          continue;
        } else {
          count += 1;
          i = cur;
          continue;
        }
      }

      // Keycap sequence: (0-9, #, *) + VS16? + U+20E3
      if ((cp >= 0x30u && cp <= 0x39u) || cp == 0x23u || cp == 0x2Au) {
        size_t peek = cur;
        unsigned int a = nextCP(peek);
        if (a == VS16) {
          unsigned int b = nextCP(peek);
          if (b == KEYCAP) {
            count += 1;
            i = peek;
            continue;
          }
        } else if (a == KEYCAP) {
          count += 1;
          i = peek;
          continue;
        }
        // otherwise treat base char normally (not an emoji)
      }

      // Emoji base or other emoji-like codepoints
      if (isEmojiBase(cp) || cp == VS16) {
        // consume optional variation selector and emoji modifier(s)
        size_t k = cur;
        unsigned int next = (k < len ? nextCP(k) : 0);
        if (next == VS16) {
          next = (k < len ? nextCP(k) : 0);
        }
        if (isEmojiModifier(next)) {
          // already advanced k past modifier
        } else {
          // reset k if we didn't actually consume anything beyond cur
          // (k will equal cur if nothing consumed)
        }

        // Handle ZWJ sequences: base (already consumed) + (ZWJ + emoji)+
        bool sawZWJ = false;
        while (true) {
          size_t zk = k;
          unsigned int z = (zk < len ? nextCP(zk) : 0);
          if (z == ZWJ) {
            unsigned int follow = (zk < len ? nextCP(zk) : 0);
            if (follow != 0 && (isEmojiBase(follow) || isRegional(follow) || follow == VS16)) {
              // advance k to zk (which is after the follow)
              k = zk;
              sawZWJ = true;
              continue;
            } else break;
          } else break;
        }

        count += 1;
        i = k;
        continue;
      }

      // Otherwise: not recognized as emoji; move on
      i = cur;
    }

    return count;
  }
}

#endif /* STR_UTILS_HPP */