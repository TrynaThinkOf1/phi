/*

 PHI
 2025/11/10

 Phi C++ Project
 src/phi/Pipeline.cpp

 Zevi Berlin

*/

#include "phi/Pipeline.hpp"

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

phi::Pipeline::Pipeline(int& erc) : sock(socket(AF_INET, SOCK_STREAM, 0)) {
  if (this->sock == -1) {
    erc = 0;
    throw std::exception{};
  }

  struct sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);

  if (connect(this->sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {  // NOLINT
    erc = 1;
    throw std::exception{};
  }
}

phi::Pipeline::~Pipeline() {
  if (this->sock >= 0) {
    this->send("</EOT>");
    close(this->sock);
  }
}

/***/

void phi::Pipeline::send(const std::string& msg) {
  if (this->sock < 0 || msg.length() == 0) return;  // NOLINT

  std::string to_send(msg.begin(), msg.end());
  while (!to_send.empty()) {
    ssize_t sent = ::send(this->sock, to_send.data(), to_send.length(), 0);
    if (sent <= 0) {  // connection dropped
      this->disconnect();
      return;
    }

    to_send = to_send.substr(sent);
  }
}

std::string phi::Pipeline::read(int num_bytes) {
  if (this->sock < 0 || num_bytes == 0) return "";  // NOLINT

  std::string received;
  received.resize(num_bytes);

  ssize_t to_read = num_bytes;
  ssize_t total_read = 0;

  while (to_read > 0) {
    ssize_t read = ::read(this->sock, received.data() + total_read, to_read);  // NOLINT -- *arithm
    if (read <= 0) {
      this->disconnect();
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

void phi::Pipeline::disconnect() {
  if (this->sock < 0) return;  // NOLINT

  this->send("</EOT>");

  close(this->sock);
  this->sock = -1;
}