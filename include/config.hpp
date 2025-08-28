/*

 PHI
 2025/08/24

 Phi C++ Project
 include/config.hpp

 Zevi Berlin

*/

#ifndef PHI_CONFIG_HPP
#define PHI_CONFIG_HPP

namespace phi {

inline constexpr int RECV_PORT = 1689;

enum ConnType {
  HANDSHAKE = 48,
  RECV_TEXT,
  RECV_FILE,

  HANDSHAKE_RECV_TEXT,
  HANDSHAKE_RECV_FILE
};

}  // namespace phi

#endif /* PHI_CONFIG_HPP */
