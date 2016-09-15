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

ChatClient::ChatClient(string server, string port) : m_run_signal(true)
{
    m_server = server;
    m_port = port;

    m_client_socket.connectWithTimeout(server.c_str(), port.c_str(), 5, 0);
}

ChatClient::~ChatClient()
{
    if (m_run_signal) {
        m_exit_mutex.lock();
        m_client_socket.sendString(EXIT_CHAT_CMD);
        m_client_socket.close();
        m_run_signal = false;
        m_exit_mutex.unlock();
    }
}

bool ChatClient::login(string user)
{
    // Store user name
    m_username = user;

    // Send greeting message
    m_client_socket.sendString(m_username);

    // Start threads
    m_display_thread.start(&doDisplay, this);
    m_read_thread.start(&doRead, this);
    m_write_thread.start(&doWrite, this);

    m_read_thread.join();
    m_write_thread.join();
    m_display_thread.join();

    m_client_socket.close();

    displayMessage("---------------------------\nConnection to server closed\n---------------------------\n");
}

void ChatClient::logout()
{
    m_client_socket.sendString(EXIT_CHAT_CMD);
    m_exit_mutex.lock();
    m_run_signal = false;
    m_exit_mutex.unlock();
}

void* ChatClient::displayLoop()
{
    string msg;
    while (m_run_signal) {
        if (!m_message_queue.empty()) {
            displayMessage(m_message_queue.front());
            m_message_queue.pop_front();
        }
    }
}

void* ChatClient::readLoop()
{
    string msg;
    while (m_run_signal) {
        if (m_client_socket.canRecv(SLEEP_DELAY)) {
            msg = m_client_socket.recvString();
            m_message_queue.push_back(msg);
            usleep(SLEEP_DELAY);
        }
    }
}

void* ChatClient::writeLoop()
{
    string outMsg;
    char outBuff[OUT_BUFFER_SIZE] = {0};

    while (m_run_signal) {
        cin.getline(outBuff, OUT_BUFFER_SIZE);

        outMsg = (string)outBuff;

        if (!outMsg.empty()) {
            if (outMsg.compare(EXIT_CHAT_CMD) == 0) {
                logout();
                return 0;
            }

            m_client_socket.sendString(outMsg);

            outMsg.clear();
            memset(outBuff, 0, OUT_BUFFER_SIZE);
        }
    }
}

void ChatClient::displayMessage(const string msg) const
{
    cout << msg;
}
