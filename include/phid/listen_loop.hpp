/*

 PHI
 2025/08/25

 Phi C++ Project
 include/phid/listen_loop.hpp

 Zevi Berlin

*/

#ifndef LISTEN_LOOP_HPP
#define LISTEN_LOOP_HPP

#include <atomic>
#include <memory>
#include <iostream>
#include <string>

#include "networking/ListenSocketSpawner.hpp"
#include "networking/ListenSocket.hpp"
#include "phid/error_handling.hpp"

namespace phi {

void listen_loop(std::atomic<bool>& should_continue);

}  // namespace phi

#endif /* LISTEN_LOOP_HPP */
