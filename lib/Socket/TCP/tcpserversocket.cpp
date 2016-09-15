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
	setsockopt(m_socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	if (::bind(m_socket_fd, hostsInfoList->ai_addr, hostsInfoList->ai_addrlen) == -1) {
		throw SocketException("Error while binding socket to port: " + port);
	}

	if (::listen(m_socket_fd, backlog) == -1) {
		throw SocketException("Error while trying to listen for connections.");
	}
}

TCPSocket TCPServerSocket::accept()
{
    struct sockaddr_storage clientAddr;

    socklen_t clientAddrSize = sizeof(clientAddr);

    int newSocketFd = ::accept(m_socket_fd, (struct sockaddr *)&clientAddr, &clientAddrSize);

    if (newSocketFd == -1) {
        throw SocketException("Error while trying to accept socket connection.");
    }

    return TCPSocket(newSocketFd);
}

void TCPServerSocket::monitor(TCPSocket &fd)
{
	struct pollfd poolFd;

    poolFd.fd = fd.getDescriptor();
    poolFd.events = POLLIN;

	m_monitor_fds.push_back(poolFd);
}

void TCPServerSocket::unmonitor(TCPSocket &fd)
{
    vector<struct pollfd>::iterator it = m_monitor_fds.begin();

    while (it != m_monitor_fds.end()) {
        if (it->fd == fd.getDescriptor()) {
            it = m_monitor_fds.erase(it);
            break;
        } else {
            ++it;
        }
    }
}

vector<TCPSocket> TCPServerSocket::getEvents(int timeout)
{
    vector<TCPSocket> events;

    int status = poll(&m_monitor_fds[0], m_monitor_fds.size(), timeout);

    if (status == -1) {
        throw SocketException("Error while pooling socket connection.");
    } else if (status == 0) {
        return events;
    } else {
    	for (auto pool_fd : m_monitor_fds) {
            if (pool_fd.revents & POLLIN) {
                pool_fd.revents = 0;
                TCPSocket sock(static_cast<int>(pool_fd.fd));
                events.push_back(sock);
            }
        }

        return events;
    }
}
