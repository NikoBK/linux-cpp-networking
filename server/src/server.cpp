#include <constants.hpp>
#include <server.hpp>
#include <message.hpp>

Server::Server(int port) : _socket(0), _connected(false), _currentSize(0)
{
    // Define the TCP _socket
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket == -1) {
        std::cerr << "Failed to create socket: " << errno << std::endl;
        exit(-1);
    }

    // set socket to reusable
    int yes = 1;
    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        std::cerr << "Failed to set reuse address: " << errno << std::endl;
        close(_serverSocket);
        exit(-1);
    }

    // get current flags
    int flags = fcntl(_serverSocket, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "Failed to get current flags: " << errno << std::endl;
        close(_serverSocket);
        exit(-1);
    }

    // enable non blocking flag
    //flags |= O_NONBLOCK;
    //if (fcntl(_serverSocket, F_SETFL, flags) == -1) {
    if (fcntl(_serverSocket, F_SETFL, fcntl(_serverSocket, F_GETFL) | O_NONBLOCK) == -1) {
        std::cerr << "Failed to set non-blocking: " << errno << std::endl;
        close(_serverSocket);
        exit(-1);
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    // bind to address and port
    if (bind(_serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Failed to bind socket." << std::endl;
        close(_serverSocket);
        exit(-1);
    }

    // backlog of 5
    if (listen(_serverSocket, 5) == -1) {
        std::cerr << "Failed to listen" << std::endl;
        close(_serverSocket);
        exit(-1);
    }
    std::cout << "Server Running on port: " << port << std::endl;
}

Server::~Server() {}
