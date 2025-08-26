/*

 PHI
 2025/08/23

 Phi C++ Project
 include/Exceptions.hpp

 Zevi Berlin

*/

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <string>
#include <netinet/in.h>

namespace phi {

class FailedSocketError : public std::exception {
  private:
    std::string message_;

  public:
    FailedSocketError(int ec)
        : message_((std::string) "Failed socket: " +
                   (std::string)strerror(ec)) {
    }

    const char* what() const noexcept {
      return message_.c_str();
    }
};

class SendFailedError : public std::exception {
  private:
    std::string message_;

  public:
    SendFailedError(int ec)
        : message_((std::string) "Send failed: " + (std::string)strerror(ec)) {
    }

    const char* what() const noexcept {
      return message_.c_str();
    }
};

class RecvFailedError : public std::exception {
  private:
    std::string message_;

  public:
    RecvFailedError(int ec)
        : message_((std::string) "Recv failed: " + (std::string)strerror(ec)) {
    }

    const char* what() const noexcept {
      return message_.c_str();
    }
};

}  // namespace phi

#endif /* EXCEPTIONS_HPP */
