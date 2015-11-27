#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include <deque>
#include <iostream>

#include "chatconfig.h"

#include "../../lib/Thread/mutex.h"
#include "../../lib/Thread/thread.h"
#include "../../lib/Socket/TCP/tcpsocket.h"

using namespace std;

class ChatClient
{
public:
	ChatClient(string server, string port = DEFAULT_PORT);
	~ChatClient();
	bool login(string username, string pass);
	void logout();

	friend void* doDisplay(void* arg);
    friend void* doRead(void* arg);
    friend void* doWrite(void* arg);
private:
	void* displayLoop();
	void* readLoop();
	void* writeLoop();
	void displayMessage(const string msg) const;

    bool runSignal_;
    TCPSocket clientSocket_;

    deque<string> messageQueue_;

	string server_, port_, username_, pass_;

	Mutex exitMutex_;
	Thread displayThread_, readThread_, writeThread_;
};

#endif // CHAT_CLIENT_H
