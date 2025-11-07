#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <thread>

// Forward declaration
class Message;

class Client
{
public:
    Client();
    ~Client();

    bool Connect(const std::string& host, int port);
    void HandleReceive();
    void Send(Message& message);
    void Disconnect(const std::string& reason);

    bool connected() const { return _connected; }

private:
    int  _socket;
    bool _connected;
    int  _currentSize;
};

#endif
