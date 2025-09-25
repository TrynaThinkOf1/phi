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

#include "phid/encryption/EncryptedMessage.hpp"

namespace phi {

struct Task {
    const EncryptedMessage message;

    const struct sockaddr_in client_address;
};

}  // namespace phi

#endif /* TASKSTRUCT_HPP */
