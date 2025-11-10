/*

 PHI
 2025/11/09

 Phi C++ Project
 include/phid/Pipeline.hpp

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

namespace phid {

class Pipeline {
  private:
    std::mutex mtx;
    std::atomic<bool> stop_listening{false};
    std::thread listen_thread;

    static constexpr int PORT = 27368;

    int sock = -1;  // active connection
    int spawner;    // sock spawner

    void listenLoop();

  public:
    Pipeline();
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

}  // namespace phid

#endif /* PIPELINE_HPP */
