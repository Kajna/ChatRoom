#include "socketexception.h"

SocketException::SocketException(const string &message)
	throw() : userMessage_(message) {
}

SocketException::~SocketException() throw() {}

const char *SocketException::what() const throw() {
	return userMessage_.c_str();
}
