#include "tcpserversocket.h"

TCPServerSocket::TCPServerSocket() : BaseSocket(AF_INET, SOCK_STREAM, AF_UNSPEC)
{

}

void TCPServerSocket::listen(string port, int backlog)
{
    struct addrinfo hostInfo;
    struct addrinfo *hostsInfoList;

    memset(&hostInfo, 0, sizeof hostInfo);

 	hostInfo.ai_family = AF_UNSPEC;
  	hostInfo.ai_socktype = SOCK_STREAM;
  	hostInfo.ai_flags = AI_PASSIVE;

	int status = getaddrinfo(NULL, port.c_str(), &hostInfo, &hostsInfoList);
	if (status != 0) {
		throw SocketException(gai_strerror(status));
	}

    int yes = 1;
	setsockopt(socketFd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	if (::bind(socketFd_, hostsInfoList->ai_addr, hostsInfoList->ai_addrlen) == -1) {
		throw SocketException("Error while binding socket to port: " + port);
	}

	if (::listen(socketFd_, backlog) == -1) {
		throw SocketException("Error while trying to listen for connections.");
	}
}

TCPSocket TCPServerSocket::accept()
{
    struct sockaddr_storage clientAddr;

    socklen_t clientAddrSize = sizeof(clientAddr);

    int newSocketFd = ::accept(socketFd_, (struct sockaddr *)&clientAddr, &clientAddrSize);

    if (newSocketFd == -1) {
        throw SocketException("Error while trying to accept socket connection.");
    }

    TCPSocket newSock(newSocketFd);
    return newSock;
}

void TCPServerSocket::monitor(TCPSocket &fd)
{
	struct pollfd poolFd;

    poolFd.fd = fd.getDescriptor();
    poolFd.events = POLLIN;

	monitorFd_.push_back(poolFd);
}

void TCPServerSocket::unmonitor(TCPSocket &fd)
{
    vector<struct pollfd>::iterator it = monitorFd_.begin();

    while (it != monitorFd_.end()) {
        if (it->fd == fd.getDescriptor()) {
            it = monitorFd_.erase(it);
            break;
        } else {
            ++it;
        }
    }
}

vector<TCPSocket> TCPServerSocket::getEvents(int timeout)
{
    vector<TCPSocket> events;

    int status = poll(&monitorFd_[0], monitorFd_.size(), timeout);

    if (status == -1) {
        throw SocketException("Error while pooling socket connection.");
    } else if (status == 0) {
        return events;
    } else {
    	for (auto poolFd : monitorFd_) {
            if (poolFd.revents & POLLIN) {
                poolFd.revents = 0;
                TCPSocket sock(static_cast<int>(poolFd.fd));
                events.push_back(sock);
            }
        }

        return events;
    }
}
