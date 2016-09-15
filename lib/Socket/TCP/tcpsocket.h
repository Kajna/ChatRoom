#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include "basesocket.h"

using namespace std;

class TCPSocket : public BaseSocket
{
public:
    explicit TCPSocket(int fd);
	explicit TCPSocket();

    bool connect(string address, string port);
    bool connectWithTimeout(string address, string port, int seconds = 3, int microseconds = 0);

	bool canRecv(int timeout = 10000);
	size_t recv(void *buff, size_t buffLen);
	size_t recvAll(void *buff, size_t buffLen);
	std::string recvString();

	size_t send(void const *buff, size_t buffLen);
	size_t sendAll(void const *buff, size_t buffLen);
	bool sendString(std::string const& msg);

	bool setSendTimeout(int seconds = 3, int microseconds = 0);
	bool setRecvTimeout(int seconds = 3, int microseconds = 0);
private:
    struct pollfd m_pool_fd;
};

#endif // TCP_SOCKET_H
