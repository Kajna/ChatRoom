#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <map>
#include <ctime>
#include <deque>
#include <vector>
#include <iostream>

#include "chatconfig.h"

#include "../../lib/Thread/mutex.h"
#include "../../lib/Thread/thread.h"
#include "../../lib/Socket/TCP/tcpserversocket.h"

using namespace std;

typedef struct Msg {
    Msg() {}
    Msg(int sock_id, string msg)
    {
        sock_id_ = sock_id;
        msg_ = msg;
    }
    int sock_id_;
    string msg_;
} ChatMsg;

typedef struct Usr {
    Usr() {}
    Usr(string id, string username, string ip)
    {
        sock_id_ = id;
        ip_ = ip;
        username_ = username;
    }
    string sock_id_, ip_, username_;
} ChatUser;

class ChatServer
{
public:
	ChatServer();
	~ChatServer();

    void start(string name);
private:
    friend void* doServerAccept(void* arg);
    friend void* doServerRead(void* arg);
    friend void* doServerWrite(void* arg);

	void* acceptLoop();
	void* readLoop();
	void* writeLoop();

	void disconnectClient(TCPSocket &client);
    void displayMessage(const string msg);

    bool runSignal_;
    TCPServerSocket serverSocket_;

    deque<ChatMsg> messageQueue_;
    vector<TCPSocket> clients_;
    map<string, ChatUser> users_;
	string name_;

	const std::string currentDateTime();

	Mutex queueMutex_;
    Thread acceptThread_, readThread_, writeThread_;
};

#endif // CHAT_SERVER_H
