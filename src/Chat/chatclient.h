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
	bool login(string username);
	void logout();

	friend void* doDisplay(void* arg);
    friend void* doRead(void* arg);
    friend void* doWrite(void* arg);
private:
	void* displayLoop();
	void* readLoop();
	void* writeLoop();
	void displayMessage(const string msg) const;

    bool m_run_signal;
    TCPSocket m_client_socket;

    deque<string> m_message_queue;

	string m_server, m_port, m_username;

	Mutex m_exit_mutex;
	Thread m_display_thread, m_read_thread, m_write_thread;
};

#endif // CHAT_CLIENT_H
