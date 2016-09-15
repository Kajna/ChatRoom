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
    Msg(int sock_id, string msg) {
        m_sock_id = sock_id;
        m_msg = msg;
    }
    int m_sock_id;
    string m_msg;
} ChatMsg;

typedef struct Usr {
    Usr() {}
    Usr(string id, string username, string ip) {
        m_sock_id = id;
        m_ip = ip;
        m_username = username;
    }
    string m_sock_id, m_ip, m_username;
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

    void displayMessage(const string msg);
	void disconnectClient(TCPSocket &client);

    const std::string currentDateTime();

    bool m_run_signal;
    TCPServerSocket m_server_socket;

    string m_room_name;
    deque<ChatMsg> m_message_queue;
    vector<TCPSocket> m_clients;
    map<string, ChatUser> m_users;

	Mutex m_queue_mutex;
    Thread m_accept_thread, m_read_thread, m_write_thread;
};

#endif // CHAT_SERVER_H
