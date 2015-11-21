#ifndef CHAT_USER_H
#define CHAT_USER_H

#include "../../lib/Socket/TCP/tcpsocket.h"

using namespace std;

class ChatUser
{
public:
	ChatUser(TCPSocket &sock, string name);
	~ChatUser();
    string name_;
    TCPSocket sock_;
};

#endif // CHAT_USER_H
