#include "basesocket.h"

BaseSocket::BaseSocket(int family, int type, int flag) : m_is_closed(false)
{
    m_socket_fd = ::socket(family, type, flag);
	if (m_socket_fd == -1) {
		throw SocketException("Error while creating socket.");
	}
}

BaseSocket::BaseSocket(int fd) : m_socket_fd(fd), m_is_closed(false)
{

}

bool BaseSocket::operator==(const BaseSocket& rhs) const
{
    return m_socket_fd == rhs.m_socket_fd;
}

int BaseSocket::getDescriptor() const
{
	return m_socket_fd;
}

void BaseSocket::close()
{
    if (m_is_closed == false) {
        ::close(m_socket_fd);
        m_is_closed = true;
    }
}

string BaseSocket::getAddress() const
{
    sockaddr_in addr;
    uint32_t addr_len = sizeof(addr);

    if (getpeername(m_socket_fd, (sockaddr *) &addr,(socklen_t *) &addr_len) < 0) {
        throw SocketException("Fetch of foreign address failed,");
    }

    std::string result(inet_ntoa(addr.sin_addr));
    return result;
}

uint32_t BaseSocket::getPort() const
{
    sockaddr_in addr;
    uint32_t addr_len = sizeof(addr);

    if (getpeername(m_socket_fd, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) {
        throw SocketException("Fetch of foreign port failed.");
    }
    return ntohs(addr.sin_port);
}

bool BaseSocket::setBlocking()
{
    int arg = fcntl(m_socket_fd, F_GETFL, NULL);
    arg &= (~O_NONBLOCK);
    fcntl(m_socket_fd, F_SETFL, arg);
}

bool BaseSocket::setNonBlocking()
{
    int arg = fcntl(m_socket_fd, F_GETFL, NULL);
    arg |= O_NONBLOCK;
    fcntl(m_socket_fd, F_SETFL, arg);
}

bool BaseSocket::isClosed()
{
    if (!m_is_closed) {
        int optval;
        socklen_t optlen = sizeof(optval);

        int res = getsockopt(m_socket_fd, SOL_SOCKET, SO_ERROR, &optval, &optlen);

        if (optval == 0 && res == 0) {
            m_is_closed = false;
        } else {
            m_is_closed = true;
        }
    }
    return m_is_closed;
}

int BaseSocket::getLastErrorCode()
{
    return errno;
}
