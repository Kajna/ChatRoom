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

ChatServer::ChatServer() : runSignal_(true)
{
    serverSocket_.listen(DEFAULT_PORT);
}

ChatServer::~ChatServer()
{

}

void* ChatServer::acceptLoop()
{
    while (runSignal_) {
        TCPSocket client = serverSocket_.accept();

        std::string port = to_string(client.getPort());

        displayMessage("User trying to connect (IP: " + client.getAddress() + ")\n");

        queueMutex_.lock();

        std::string username = client.recvString();

        if (!username.empty()) {
            displayMessage("User " + username + " connected (IP: " + client.getAddress() + ")\n");

            client.sendString("---------------------------\n Welcome to chat room "
            + name_
            + "\n---------------------------\n");

            serverSocket_.monitor(client);
            clients_.push_back(client);

            ChatUser user(to_string(client.getDescriptor()), username, client.getAddress());

            users_[user.ip_ + ":" + user.sock_id_] = user;
        } else {
            client.close();
        }

        queueMutex_.unlock();

        usleep(SLEEP_DELAY);
    }
}

void* ChatServer::readLoop()
{
    while (runSignal_) {
        if (!clients_.empty()) {

            vector<TCPSocket> events = serverSocket_.getEvents(SLEEP_DELAY);

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
                    + users_[client.getAddress() + ":" + to_string(client.getDescriptor())].username_
                    + "> "
                    + msg
                    + "\n");
                    messageQueue_.push_back(m);
                    displayMessage(m.msg_);
                }
            }
            events.empty();
        }
        usleep(SLEEP_DELAY);
    }
}

void* ChatServer::writeLoop()
{
    while (runSignal_) {
        if (!messageQueue_.empty()) {

            ChatMsg msg = messageQueue_.front();

            queueMutex_.lock();

            for (auto &client : clients_) {
                if (client.getDescriptor() != msg.sock_id_ && !client.sendString(msg.msg_)) {
                    queueMutex_.unlock();
                    disconnectClient(client);
                    queueMutex_.lock();
                }
            }

            messageQueue_.pop_front();

            queueMutex_.unlock();
        }
        usleep(SLEEP_DELAY);
    }
}

void ChatServer::start(string name)
{
    name_ = name;

    displayMessage("---------------------------\nChat room server is running... \n---------------------------\n");

    // Start threads
    acceptThread_.start(&doServerAccept, this);
    readThread_.start(&doServerRead, this);
    writeThread_.start(&doServerWrite, this);

    writeThread_.join();
    readThread_.join();
    acceptThread_.join();

    serverSocket_.close();

    displayMessage("Server closed\n");
}

void ChatServer::disconnectClient(TCPSocket &client)
{
    queueMutex_.lock();

    serverSocket_.unmonitor(client);

    client.close();

    std::vector<TCPSocket>::iterator it = clients_.begin();

    while (it != clients_.end()) {
        if (it->getDescriptor() == client.getDescriptor()) {
            ChatUser user = users_[it->getAddress() + ":" + to_string(it->getDescriptor())];
            it = clients_.erase(it);
            displayMessage("User " + user.username_ + " disconected (IP: " + user.ip_ + ")\n");
            break;
        } else {
            ++it;
        }
    }

    queueMutex_.unlock();
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
