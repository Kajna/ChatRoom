#include "basesocket.h"

BaseSocket::BaseSocket(int family, int type, int flag) : isClosed_(false)
{
    socketFd_ = ::socket(family, type, flag);
	if (socketFd_ == -1) {
		throw SocketException("Error while creating socket.");
	}
}

BaseSocket::BaseSocket(int fd) : socketFd_(fd), isClosed_(false)
{

}

bool BaseSocket::operator==(const BaseSocket& rhs) const
{
    return socketFd_ == rhs.socketFd_;
}

int BaseSocket::getDescriptor() const
{
	return socketFd_;
}

void BaseSocket::close()
{
    if (isClosed_ == false) {
        ::close(socketFd_);
        isClosed_ = true;
    }
}

string BaseSocket::getAddress() const
{
    sockaddr_in addr;
    uint32_t addr_len = sizeof(addr);

    if (getpeername(socketFd_, (sockaddr *) &addr,(socklen_t *) &addr_len) < 0) {
        throw SocketException("Fetch of foreign address failed,");
    }

    std::string result(inet_ntoa(addr.sin_addr));
    return result;
}

uint32_t BaseSocket::getPort() const
{
    sockaddr_in addr;
    uint32_t addr_len = sizeof(addr);

    if (getpeername(socketFd_, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) {
        throw SocketException("Fetch of foreign port failed.");
    }
    return ntohs(addr.sin_port);
}

bool BaseSocket::setBlocking()
{
    int arg = fcntl(socketFd_, F_GETFL, NULL);
    arg &= (~O_NONBLOCK);
    fcntl(socketFd_, F_SETFL, arg);
}

bool BaseSocket::setNonBlocking()
{
    int arg = fcntl(socketFd_, F_GETFL, NULL);
    arg |= O_NONBLOCK;
    fcntl(socketFd_, F_SETFL, arg);
}

bool BaseSocket::isClosed()
{
    if (!isClosed_) {
        int optval;
        socklen_t optlen = sizeof(optval);

        int res = getsockopt(socketFd_, SOL_SOCKET, SO_ERROR, &optval, &optlen);

        if (optval == 0 && res == 0) {
            isClosed_ = false;
        } else {
            isClosed_ = true;
        }
    }
    return isClosed_;
}
