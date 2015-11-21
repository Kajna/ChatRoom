#include "chatuser.h"

ChatUser::ChatUser(TCPSocket &sock, string name) : sock_(std::move(sock)), name_(name)
{
    //ctor
}

ChatUser::~ChatUser()
{
    //dtor
}
