#include "socketexception.h"

SocketException::SocketException(const string &message)
	throw() : m_user_message(message) {
}

SocketException::~SocketException() throw() {}

const char *SocketException::what() const throw() {
	return m_user_message.c_str();
}
