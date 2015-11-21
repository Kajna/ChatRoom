#ifndef BASE_SOCKET_H
#define BASE_SOCKET_H

#include <sys/types.h>       // For data types
#include <sys/socket.h>      // For socket(), connect(), send(), and recv()
#include <arpa/inet.h>       // For inet_addr()
#include <unistd.h>          // For close()
#include <netinet/in.h>      // For sockaddr_in
#include <poll.h>            // For poll()
#include <fcntl.h>           // For fnctl()
#include <netdb.h>           // For gethostbyname()
#include <cstring> 			 // For memset()

#include "../socketexception.h"

typedef void rawType;        // Type used for raw data on this platform

using namespace std;

class BaseSocket
{
public:
    BaseSocket(int family, int type, int flag);
    explicit BaseSocket(int fd);

	bool operator==(const BaseSocket& rhs) const;

	string getAddress() const;
    uint32_t getPort() const;

    int getDescriptor() const;

	void close();
	bool isClosed();

	bool setBlocking();
	bool setNonBlocking();

	int getLastErrorCode();
protected:
    int socketFd_;
    bool isClosed_;
};

#endif // BASE_SOCKET_H
