#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <map>
#include <deque>
#include <vector>
#include <iostream>

#include "chatuser.h"
#include "chatconfig.h"

#include "../../lib/Thread/mutex.h"
#include "../../lib/Thread/thread.h"
#include "../../lib/Socket/TCP/tcpserversocket.h"

using namespace std;

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

    deque<string> messageQueue_;
    vector<TCPSocket> clients_;
    map<int, ChatUser> users_;
	string name_;

	Mutex queueMutex_;
    Thread acceptThread_, readThread_, writeThread_;
};

#endif // CHAT_SERVER_H
