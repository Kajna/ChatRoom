#ifndef TCP_SERVER_SOCKET_H
#define TCP_SERVER_SOCKET_H

#include <vector>

#include "tcpsocket.h"

using namespace std;

class TCPServerSocket : public BaseSocket
{
public:
	explicit TCPServerSocket();

    void listen(string port, int backlog = 5);
	TCPSocket accept();
	void monitor(TCPSocket &fd);
	void unmonitor(TCPSocket &fd);
	vector<TCPSocket> getEvents(int timeout = 10000);
private:
    vector<struct pollfd> monitorFd_;
};

#endif // TCP_SERVER_SOCKET_H
