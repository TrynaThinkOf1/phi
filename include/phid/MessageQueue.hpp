/*

 PHI
 2025/08/22

 Phi C++ Project
 include/MessageQueue.hpp

 Zevi Berlin

*/

#ifndef MESSAGEQUEUE_HPP
#define MESSAGEQUEUE_HPP

#include <vector>
#include <string>
#include <netinet/in.h>

namespace phi {

struct Message {
    int msgId;
    std::string msgContent;
    struct sockaddr_in clientAddress;
};

}  // namespace phi

#endif /* MESSAGEQUEUE_HPP */
