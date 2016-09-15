#include "chatserver.h"

void* doServerAccept(void* arg)
{
    return static_cast<ChatServer*>(arg)->acceptLoop();
}

void* doServerRead(void* arg)
{
    return static_cast<ChatServer*>(arg)->readLoop();
}

void* doServerWrite(void* arg)
{
    return static_cast<ChatServer*>(arg)->writeLoop();
}

ChatServer::ChatServer() : m_run_signal(true)
{
    m_server_socket.listen(DEFAULT_PORT);
}

ChatServer::~ChatServer()
{

}

void* ChatServer::acceptLoop()
{
    while (m_run_signal) {
        TCPSocket client = m_server_socket.accept();

        std::string port = to_string(client.getPort());

        displayMessage("User trying to connect (IP: " + client.getAddress() + ")\n");

        m_queue_mutex.lock();

        std::string username = client.recvString();

        if (!username.empty()) {
            displayMessage("User " + username + " connected (IP: " + client.getAddress() + ")\n");

            client.sendString("---------------------------\n Welcome to chat room "
            + m_room_name
            + "\n---------------------------\n");

            m_server_socket.monitor(client);
            m_clients.push_back(client);

            ChatUser user(to_string(client.getDescriptor()), username, client.getAddress());

            m_users[user.m_ip + ":" + user.m_sock_id] = user;
        } else {
            client.close();
        }

        m_queue_mutex.unlock();

        usleep(SLEEP_DELAY);
    }
}

void* ChatServer::readLoop()
{
    while (m_run_signal) {
        if (!m_clients.empty()) {

            vector<TCPSocket> events = m_server_socket.getEvents(SLEEP_DELAY);

            for (auto &client: events) {
                string msg = client.recvString();
                if (msg.empty() || msg.find(EXIT_CHAT_CMD) == 0) {
                    disconnectClient(client);
                } else {
                    ChatMsg m(client.getDescriptor(),
                    "["
                    + currentDateTime()
                    + "] "
                    + "<"
                    + m_users[client.getAddress() + ":" + to_string(client.getDescriptor())].m_username
                    + "> "
                    + msg
                    + "\n");
                    m_message_queue.push_back(m);
                    displayMessage(m.m_msg);
                }
            }
            events.empty();
        }
        usleep(SLEEP_DELAY);
    }
}

void* ChatServer::writeLoop()
{
    while (m_run_signal) {
        if (!m_message_queue.empty()) {

            ChatMsg msg = m_message_queue.front();

            m_queue_mutex.lock();

            for (auto &client : m_clients) {
                if (client.getDescriptor() != msg.m_sock_id && !client.sendString(msg.m_msg)) {
                    m_queue_mutex.unlock();
                    disconnectClient(client);
                    m_queue_mutex.lock();
                }
            }

            m_message_queue.pop_front();

            m_queue_mutex.unlock();
        }
        usleep(SLEEP_DELAY);
    }
}

void ChatServer::start(string name)
{
    m_room_name = name;

    displayMessage("---------------------------\nChat room server is running... \n---------------------------\n");

    // Start threads
    m_accept_thread.start(&doServerAccept, this);
    m_read_thread.start(&doServerRead, this);
    m_write_thread.start(&doServerWrite, this);

    m_write_thread.join();
    m_read_thread.join();
    m_accept_thread.join();

    m_server_socket.close();

    displayMessage("Server closed\n");
}

void ChatServer::disconnectClient(TCPSocket &client)
{
    m_queue_mutex.lock();

    std::vector<TCPSocket>::iterator it = m_clients.begin();

    while (it != m_clients.end()) {
        if (it->getDescriptor() == client.getDescriptor()) {
            ChatUser user = m_users[it->getAddress() + ":" + to_string(it->getDescriptor())];

            displayMessage("User " + user.m_username + " disconected (IP: " + user.m_ip + ")\n");

            it = m_clients.erase(it);
            m_users.erase(it->getAddress() + ":" + to_string(it->getDescriptor()));

            m_server_socket.unmonitor(client);
            client.close();
            break;
        } else {
            ++it;
        }
    }

    m_queue_mutex.unlock();
}

void ChatServer::displayMessage(const string msg)
{
    cout << msg;
}

const std::string ChatServer::currentDateTime()
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format %Y-%m-%d.
    strftime(buf, sizeof(buf), "%X", &tstruct);

    return buf;
}
