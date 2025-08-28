/*

 PHI
 2025/08/23

 Phi C++ Project
 include/phid/error_handling.hpp

 Zevi Berlin

*/

#ifndef ERROR_HANDLING_HPP
#define ERROR_HANDLING_HPP

#include <string>
#include <iostream>

namespace phi {

inline void handle_error(std::string msg) {
  std::cout << "Error: " << msg << std::endl;
  exit(-1);
}

}  // namespace phi

#endif /* ERROR_HANDLING_HPP */
