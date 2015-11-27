#include "chatclient.h"

void* doDisplay(void* arg)
{
    return static_cast<ChatClient*>(arg)->displayLoop();
}

void* doRead(void* arg)
{
    return static_cast<ChatClient*>(arg)->readLoop();
}

void* doWrite(void* arg)
{
    return static_cast<ChatClient*>(arg)->writeLoop();
}

ChatClient::ChatClient(string server, string port) : runSignal_(true)
{
    server_ = server;
    port_ = port;

    clientSocket_ = new TCPSocket();

    clientSocket_->connectWithTimeout(server.c_str(), port.c_str(), 5, 5);
}

ChatClient::~ChatClient()
{
    if (runSignal_) {
        exitMutex_.lock();
        clientSocket_->sendString(EXIT_CHAT_CMD);
        clientSocket_->close();
        runSignal_ = false;
        exitMutex_.unlock();
        delete clientSocket_;
    }
}

bool ChatClient::login(string user, string pass)
{
    // Store user name password and send greet msg
    username_ = user;
    pass_ = pass;

    // Login
    clientSocket_->sendString(username_);

    // Start threads
    displayThread_.start(&doDisplay, this);
    readThread_.start(&doRead, this);
    writeThread_.start(&doWrite, this);

    readThread_.join();
    writeThread_.join();
    displayThread_.join();

    clientSocket_->close();

    displayMessage("---------------------------\nConnection to server closed\n---------------------------\n");
}

void ChatClient::logout()
{
    clientSocket_->sendString(EXIT_CHAT_CMD);
    exitMutex_.lock();
    runSignal_ = false;
    exitMutex_.unlock();
}

void* ChatClient::displayLoop()
{
    string msg;
    while (runSignal_) {
        if (!messageQueue_.empty()) {
            msg = messageQueue_.front();
            if (!isSameMessage(msg)) {
                displayMessage(msg);
            }
            messageQueue_.pop_front();
        }
    }
}

void* ChatClient::readLoop()
{
    string msg;
    while (runSignal_) {
        if (clientSocket_->canRecv(SLEEP_DELAY)) {
            msg = clientSocket_->recvString();
            messageQueue_.push_back(msg);
            usleep(SLEEP_DELAY);
        }
    }
}

void* ChatClient::writeLoop()
{
    string outMsg;
    char outBuff[OUT_BUFFER_SIZE] = {0};

    while (runSignal_) {
        cin.getline(outBuff, OUT_BUFFER_SIZE);

        outMsg = (string)outBuff;

        if (!outMsg.empty()) {
            if (outMsg.compare(EXIT_CHAT_CMD) == 0) {
                logout();
                return 0;
            }

            clientSocket_->sendString(username_ + ": " + outMsg + '\n');

            outMsg.clear();
            memset(outBuff, 0, OUT_BUFFER_SIZE);
        }
    }
}

bool ChatClient::isSameMessage(const string msg) const
{
    if (msg.find(username_) != string::npos) {
		return true;
	} else {
		return false;
    }
}

void ChatClient::displayMessage(const string msg) const
{
    cout << msg;
}
