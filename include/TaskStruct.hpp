/*

 PHI
 2025/09/19

 Phi C++ Project
 include/TaskStruct.hpp

 Zevi Berlin

*/

#ifndef TASKSTRUCT_HPP
#define TASKSTRUCT_HPP

#include <string>
#include <netinet/in.h>

namespace phi {

struct Task {
    const std::string encrypted_message;
    const int msg_split = message.length() / 1024;

    const std::string client_key;

    const struct sockaddr_in client_address;
};

}  // namespace phi

#endif /* TASKSTRUCT_HPP */
