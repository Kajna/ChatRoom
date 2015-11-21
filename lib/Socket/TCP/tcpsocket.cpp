#include "tcpsocket.h"

TCPSocket::TCPSocket(int fd) : BaseSocket(fd)
{
    poolFd_.fd = socketFd_;
    poolFd_.events = POLLIN;
}

TCPSocket::TCPSocket() : BaseSocket(AF_INET, SOCK_STREAM, AF_UNSPEC)
{

}

bool TCPSocket::connect(string address, string port)
{
    struct addrinfo hostInfo;
    struct addrinfo *hostsInfoList;

    memset(&hostInfo, 0, sizeof hostInfo);

 	hostInfo.ai_family = AF_UNSPEC;
  	hostInfo.ai_socktype = SOCK_STREAM;
  	hostInfo.ai_flags = AF_UNSPEC;

    int status = getaddrinfo(address.c_str(), port.c_str(), &hostInfo, &hostsInfoList);
	if (status != 0) {
		throw SocketException(gai_strerror(status));
	}

    status = ::connect(socketFd_, hostsInfoList->ai_addr, hostsInfoList->ai_addrlen);

    freeaddrinfo(hostsInfoList);

    if (status == -1) {
        throw SocketException("Error while connecting socket to: " + address + ":" + port);
    }

    poolFd_.fd = socketFd_;
    poolFd_.events = POLLIN;
}

bool TCPSocket::connectWithTimeout(string address, string port, int seconds, int microseconds)
{
    fd_set fdset;
    struct timeval tv;

    // Set non-blocking
    setNonBlocking();

    struct addrinfo hostInfo;
    struct addrinfo *hostsInfoList;

    memset(&hostInfo, 0, sizeof hostInfo);

 	hostInfo.ai_family = AF_UNSPEC;
  	hostInfo.ai_socktype = SOCK_STREAM;
  	hostInfo.ai_flags = AF_UNSPEC;

    int status = getaddrinfo(address.c_str(), port.c_str(), &hostInfo, &hostsInfoList);
	if (status != 0) {
		throw SocketException(gai_strerror(status));
	}

    ::connect(socketFd_, hostsInfoList->ai_addr, hostsInfoList->ai_addrlen);

    freeaddrinfo(hostsInfoList);

    FD_ZERO(&fdset);
    FD_SET(socketFd_, &fdset);

    tv.tv_sec = seconds;
    tv.tv_usec = microseconds;

    if (select(socketFd_ + 1, NULL, &fdset, NULL, &tv) == 1) {
        int so_error;

        socklen_t len = sizeof so_error;

        getsockopt(socketFd_, SOL_SOCKET, SO_ERROR, &so_error, &len);

        if (so_error != 0) {
            throw SocketException("Error while connecting socket (timeout) to: " + address + ":" + port);
        }
    } else {
        throw SocketException("Error while connecting socket (timeout) to: " + address + ":" + port);
    }

    // Set to blocking mode again...
    setBlocking();

    poolFd_.fd = socketFd_;
    poolFd_.events = POLLIN;
}


bool TCPSocket::canRecv(int timeout)
{
    int status = poll(&poolFd_, 1, timeout);

    if (status == -1) {
        throw SocketException("Error while pooling socket connection.");
    } else if (status == 0) {
        return false;
    } else {
        if (poolFd_.revents & POLLIN) {
            poolFd_.revents = 0;
            return true;
        }
    }
    return false;
}

size_t TCPSocket::recv(void *buff, size_t buffLen)
{
    return ::recv(socketFd_, (rawType *)buff, buffLen, 0);
}

size_t TCPSocket::recvAll(void *buff, size_t buffLen)
{
    size_t result, total = 0;
    while (total < buffLen) {
        result = ::recv(socketFd_, (rawType *)buff, buffLen, 0);
        if (result < 0) {
            throw SocketException("Error while receiving data from socket.");
        } else if (result == 0) {
            break;
        }
        buff = static_cast<char*>(buff) + result;
        total += result;
    }
    return total;
}

std::string TCPSocket::recvString()
{
    // Receive message length in network byte order
    uint32_t len;
    if (recvAll(&len, sizeof len) != (sizeof len)) {
        return std::string();
    }
    // Receive the message
    len = ntohl(len);
    std::string msg(len, '\0');
    if (recvAll(&msg[0], len) != len) {
        return std::string();
    }

    // Return received string
    return msg;
}

size_t TCPSocket::send(void const *buff, size_t buffLen)
{
    return ::recv(socketFd_, (rawType *)buff, buffLen, 0);
}

size_t TCPSocket::sendAll(void const *buff, size_t buffLen)
{
    size_t result, total = 0;
    while (total < buffLen) {
        result = ::send(socketFd_, (rawType *)buff, buffLen, 0);
        if (result < 0) {
            throw SocketException("Error while sending data to socket.");
        } else if (result == 0) {
            break;
        }
        buff = static_cast<char const*>(buff) + result;
        total += result;
    }
    return total;
}

bool TCPSocket::sendString(std::string const& msg)
{
    bool success = true;

    // Get message lenght in network byte order
    uint32_t len = msg.size();
    len = htonl(len);

    // Send message length
    if (sendAll(&len, sizeof len) != (sizeof len)) {
        success = false;
    }

    // Send the message
    if (success == true && sendAll(msg.data(), msg.size()) != msg.size()) {
        success = false;
    }

    return success;
}

bool TCPSocket::setSendTimeout(int seconds, int microseconds)
{
    struct timeval timeout;
    timeout.tv_sec = seconds;
    timeout.tv_usec = microseconds;

    if (setsockopt (socketFd_, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        return false;
    }
    return true;
}

bool TCPSocket::setRecvTimeout(int seconds, int microseconds)
{
    struct timeval timeout;
    timeout.tv_sec = seconds;
    timeout.tv_usec = microseconds;

    if (setsockopt (socketFd_, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        return false;
    }

    return true;
}
