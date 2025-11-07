#include <iostream>
#include <cstring>
#include <client.hpp>
#include <message.hpp>
#include <constants.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

Client::Client()
: _socket(-1), _connected(false), _currentSize(0)
{}

Client::~Client()
{
    if (_connected) {
        close(_socket);
    }
}

bool Client::Connect(const std::string& host, int port)
{
    // create socket
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1) {
        std::cerr << "Failed to create socket: " << errno << "\n";
        return false;
    }

    // set non-blocking
    int flags = fcntl(_socket, F_GETFL, 0);
    fcntl(_socket, F_SETFL, flags | O_NONBLOCK);

    // server address
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) != 1) {
        std::cerr << "Invalid address\n";
        close(_socket);
        return false;
    }

    // attempt connect
    int result = connect(_socket, (sockaddr*)&addr, sizeof(addr));

    if (result == 0) {
        // connected instantly
        std::cout << "Succesfully connected to " << host << ":" << port << "\n";
        _connected = true;
        return true;
    }

    // non-blocking in-progress connect
    if (result == -1 && errno == EINPROGRESS) {
        std::cout << "Connecting to " << host << ":" << port << "...\n";
        // loop will call HandleReceive until fully connected
        _connected = true;  // mark as "attempting"
        std::cout << "Succesfully connected to " << host << ":" << port << "\n";
        return true;
    }

    std::cerr << "Connect failed: " << strerror(errno) << "\n";
    close(_socket);
    return false;
}

void Client::HandleReceive()
{
    if (!_connected)
        return;

    // peek size
    if (_currentSize == 0)
    {
        int bytesReadable = 0;
        int result = recv(_socket, reinterpret_cast<char*>(&bytesReadable), sizeof(int), MSG_PEEK);

        if (result <= 0) {
            if (errno == EWOULDBLOCK)
                return;

            // socket closed
            Disconnect("connection lost");
            return;
        }

        if (result < sizeof(int))
            return;

        _currentSize = ntohl(bytesReadable);
        if (_currentSize <= 0 || _currentSize > 8192) {
            Disconnect("invalid message size");
            return;
        }
    }

    // read full message
    std::vector<char> buffer(_currentSize);
    int result = recv(_socket, buffer.data(), _currentSize, 0);

    if (result <= 0) {
        if (errno == EWOULDBLOCK)
            return;

        Disconnect("recv failed");
        return;
    }

    // decode (you implement Decoder)
    Decoder decoder(buffer.data() + 4, _currentSize);

    unsigned char messageId;
    decoder.ReadByte(&messageId);

    std::cout << "Client got message ID = " << (int)messageId << "\n";
    switch ((int)messageId)
    {
        case constants::reply_id: {
            ReplyMessage msg;
            msg.decode(decoder);

            std::cout << "Server says:\nText: " << msg.text << "\nResult: " << msg.result << "\nSolved? - " << msg.solved << std::endl;
            std::cout << "PS: Message float value is: " << msg.test << std::endl;
        }
    }

    _currentSize = 0;
}

void Client::Send(Message& message)
{
    if (!_connected)
        return;

    Encoder encoder;
    message.encode(encoder);

    const char* buf = encoder.buffer();
    int size = encoder.size();

    int result = send(_socket, buf, size, 0);
    if (result <= 0) {
        if (errno != EWOULDBLOCK) {
            Disconnect("send failed");
        }
    }
}

void Client::Disconnect(const std::string& reason)
{
    if (!_connected)
        return;

    std::cout << "Client disconnect: " << reason << "\n";

    close(_socket);
    _socket = -1;
    _connected = false;
    _currentSize = 0;
}
