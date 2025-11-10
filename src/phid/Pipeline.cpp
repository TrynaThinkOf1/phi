/*

 PHI
 2025/11/09

 Phi C++ Project
 src/phid/pipeline.cpp

 Zevi Berlin

*/

#include "phid/Pipeline.hpp"

#include <atomic>
#include <thread>
#include <mutex>
#include <string>
#include <chrono>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

phid::Pipeline::Pipeline() : spawner(socket(AF_INET, SOCK_STREAM, 0)) {
  if (this->spawner == -1) {
    throw std::runtime_error("Failed to create pipeline socket");
  }

  int flags = fcntl(this->spawner, F_GETFL, 0);
  fcntl(this->spawner, F_SETFL, flags | O_NONBLOCK);  // NOLINT

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

  if (bind(this->spawner, (struct sockaddr*)&addr, sizeof(addr)) == -1) {  // NOLINT
    close(this->spawner);
    throw std::runtime_error("Failed to bind pipeline socket");
  }

  if (listen(this->spawner, 1) == -1) {
    close(this->spawner);
    throw std::runtime_error("Failed to listen on pipeline socket");
  }
  this->listen_thread = std::thread(&phid::Pipeline::listenLoop, this);
}

phid::Pipeline::~Pipeline() {
  this->stop_listening = true;

  shutdown(this->spawner, SHUT_RDWR);
  if (this->listen_thread.joinable()) this->listen_thread.join();  // NOLINT
  close(this->spawner);

  if (this->sock >= 0) {
    this->send("</EOF>");
    close(this->sock);
  }
}

/***/

void phid::Pipeline::listenLoop() {
  while (!this->stop_listening) {
    if (this->sock <= 0) {
      struct pollfd pfd{};
      pfd.fd = this->spawner;
      pfd.events = POLLIN;

      int result = poll(&pfd, 1, 100);  // NOLINT -- 100ms timeout

      if (result < 0) {
        if (this->stop_listening) break;  // NOLINT
        continue;
      }
      if (result == 0) {
        // timeout, nothing happened
        continue;
      }

      int client_sock = accept(this->spawner, nullptr, nullptr);

      if (client_sock < 0) {
        if (this->stop_listening) break;  // NOLINT
        continue;
      }

      {
        std::lock_guard<std::mutex> lock(this->mtx);

        this->sock = client_sock;
      }
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));  // NOLINT -- magic number
    }
  }
}

/***/

void phid::Pipeline::send(const std::string& msg) {
  std::lock_guard<std::mutex> lock(this->mtx);

  if (this->sock < 0 || msg.length() == 0) return;  // NOLINT

  std::string to_send(msg.begin(), msg.end());
  while (!to_send.empty()) {
    ssize_t sent = ::send(this->sock, to_send.data(), to_send.length(), 0);
    if (sent <= 0) {  // connection dropped
      close(this->sock);
      this->sock = -1;
      return;
    }

    to_send = to_send.substr(sent);
  }
}

std::string phid::Pipeline::read(int num_bytes) {
  std::lock_guard<std::mutex> lock(this->mtx);

  if (this->sock < 0 || num_bytes == 0) return "";  // NOLINT

  std::string received;
  received.resize(num_bytes);

  ssize_t to_read = num_bytes;
  ssize_t total_read = 0;

  while (to_read > 0) {
    ssize_t read = ::read(this->sock, received.data() + total_read, to_read);  // NOLINT -- *arithm
    if (read <= 0) {
      close(this->sock);
      this->sock = -1;
      received.resize(total_read);
      return received;
    }

    to_read -= read;
    total_read += read;
  }

  if (total_read < num_bytes) {
    received.resize(total_read);
  }
  return received;
}

/***/

void phid::Pipeline::disconnect() {
  std::lock_guard<std::mutex> lock(this->mtx);

  if (this->sock < 0) return;  // NOLINT

  std::string eof_msg = "</EOF>";
  ::send(this->sock, eof_msg.data(), eof_msg.size(), 0);

  close(this->sock);
  this->sock = -1;
}