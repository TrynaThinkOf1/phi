/*

 PHI
 2025/11/10

 Phi C++ Project
 include/phi/Pipeline.hpp

 Zevi Berlin

*/

#ifndef PIPELINE_HPP
#define PIPELINE_HPP


#include <atomic>
#include <thread>
#include <mutex>
#include <string>
#include <chrono>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

namespace phi {

class Pipeline {
  private:
    static constexpr int PORT = 27368;

    int sock = -1;  // active connection

  public:
    Pipeline(int& erc);
    ~Pipeline();

    /***/

    std::string read(int num_bytes);
    void send(const std::string& msg);

    /***/

    void disconnect();

    bool isConnected() const {
      return this->sock >= 0;
    }
};

}  // namespace phi

#endif /* PIPELINE_HPP */

