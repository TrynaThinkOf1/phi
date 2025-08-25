/*

 PHI
 2025/08/24

 Phi C++ Project
 src/phid/ListenSocket.cpp

 Zevi Berlin

*/

#include "networking/ListenSocket.hpp"

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Exceptions.hpp"

void phi::ListenSocket::recv(std::string& buffer) {
}

void phi::ListenSocket::send(const std::string& buf) {
	int ec;
	for (int i = 0; i < 3; i++) {
		ec = send(sock, buf, buf.length(), 0) 
		if (ec > 0) {
			return; // it was successful so early return
		}
	}

	// it did not send properly within 3 tries
	throw phi::SendFailedError(ec);
}
