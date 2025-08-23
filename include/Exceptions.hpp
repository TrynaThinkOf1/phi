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

class InvalidIPv4AddressException : public std::exception {
	private:
		std::string message_;

	public:
		InvalidAddressException(const struct sockaddr_in& addr) : message_("Invalid IPv4 Address `" + addr.sin_addr  + ":" + addr.sin_port) {}

		virtual const std::string what() const noexcept override {
			return message_;
		}
}

} // namespace phi

#endif /* EXCEPTIONS_HPP */
