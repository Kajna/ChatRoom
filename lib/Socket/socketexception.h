#ifndef SOCKETEXCEPTION_H
#define SOCKETEXCEPTION_H

#include <string>
#include <exception>

using namespace std;

class SocketException : public exception
{
public:
	SocketException(const string &message) throw();
	~SocketException() throw();
	const char *what() const throw();

private:
	string userMessage_;  // Exception message
};

#endif // SOCKETEXCEPTION_H
