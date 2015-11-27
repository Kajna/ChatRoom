#ifndef CHAT_USER_H
#define CHAT_USER_H

#include<string>

using namespace std;

class ChatUser
{
public:
	ChatUser();
	ChatUser(string name);
	~ChatUser();

    string username_;
    string ip_;
};

#endif // CHAT_USER_H
