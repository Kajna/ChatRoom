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

ChatServer::ChatServer() : serverSocket_(), runSignal_(true)
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

        displayMessage("Client connected (IP: " + client.getAddress() + " Port: " + port.c_str() + ")\n");

        queueMutex_.lock();

        client.sendString("---------------------------\n Welcome to chat room "
        + name_
        + "\n---------------------------\n");

        serverSocket_.monitor(client);
        client.setSendTimeout(1);
        client.setRecvTimeout(1);
        clients_.push_back(client);
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
                    messageQueue_.push_back(msg);
                    displayMessage("ChatLog: " + msg);
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

            string msg = messageQueue_.front();

            queueMutex_.lock();

            for (auto &client : clients_) {
                if (!client.sendString(msg)) {
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
            it = clients_.erase(it);
            break;
        } else {
            ++it;
        }
    }
    queueMutex_.unlock();

    displayMessage("Client disconected\n");
}

void ChatServer::displayMessage(const string msg)
{
    cout << msg;
}