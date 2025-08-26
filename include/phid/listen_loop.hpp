/*

 PHI
 2025/08/25

 Phi C++ Project
 include/phid/listen_loop.hpp

 Zevi Berlin

*/

#ifndef LISTEN_LOOP_HPP
#define LISTEN_LOOP_HPP

#include <iostream>
#include <string>
#include <netinet/in.h>

#include "networking/ListenSocketSpawner.hpp"
#include "networking/ListenSocket.hpp"

namespace phi {

void listen_loop();

}  // namespace phi

#endif /* LISTEN_LOOP_HPP */
