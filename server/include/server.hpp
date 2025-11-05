#ifndef SERVER_HPP
#define SERVER_HPP

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <errno.h>

#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string>

// Forward decleration
class Message;

class Server
{
public:
    Server(int port);
    ~Server(); // server class destructor

    bool connected() const { return _connected; }

    void AcceptConnection();
    void HandleConnection(int *state);
    void Send(Message& message);
    void Disconnect(const std::string& reason);
    void SendError(std::string text);

private:
    int _socket;
    bool _connected;
    int _currentSize;
    int _serverSocket;
};

#endif
