#include <string>
#include <iostream>
#include <algorithm>

#include "Chat/chatclient.h"
#include "Chat/chatserver.h"

using namespace std;

void startServer();
void startClient();

int main()
{
    int type = 1, exit = 1;

    cout << "---------------------------\n  CHAT ROOM  \n---------------------------" << endl;

    while (exit) {
        try {
            cout << "Choose? \n  [1] -> Client \n  [2] -> Server  \n  [3] -> Help  \n  [4] -> Exit" << endl;

            cin >> type;

            switch (type) {
                case 1:
                    startClient();
                    break;
                case 2:
                    startServer();
                    break;
                case 3:
                    cout << "Commands:\n /exit -> Exit current chat room\n /file [filepath] Send file to" << endl;
                    break;
                case 4:
                    exit = 0;
                    break;
                default:
                cout << "Uknown command!" << endl;
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        } catch(exception& e) {
            cout << e.what() << endl;
        }
    }

    return 0;
}

void startClient()
{
    string user, server;

    cout << "Enter username?" << endl;
    cin >> user;

    cout << "Enter server address?" << endl;
    cin >> server;

    ChatClient c(server, "5555");

    c.login(user);
}

void startServer()
{
    string name;

    cout << "Enter room name?" << endl;
    cin >> name;

    ChatServer s;

    s.start(name);
}

