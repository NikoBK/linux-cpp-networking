#include <iostream>
#include <atomic>
#include <chrono>
#include <thread>

#include <constants.hpp>
#include <client.hpp>
#include <message.hpp>

int main() {
    std::atomic<bool> running = true;

    Client client;

    const std::string host = "127.0.0.1";
    int port = constants::server_port;

    bool connected = client.Connect(host, port);
    if (connected) {
        HelloMessage msg;
        msg.text = "Hello server, this is the client!";
        msg.addA = 2;
        msg.addB = 7;
        msg.solved = false;
        client.Send(msg);
    }

    // 50 Hz tick loop
    const auto tick = std::chrono::milliseconds(20);

    while (running)
    {
        if (!client.connected()) {
            client.Connect(host, port);
        }
        else {
            client.HandleReceive();
        }
        std::this_thread::sleep_for(tick);
    }

    return 0;
}
