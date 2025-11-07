#include <iostream>
#include <atomic>
#include <chrono>
#include <thread>

#include <constants.hpp>
#include <server.hpp>

int main() {
    std::atomic<bool> running = true;

    Server server(constants::server_port);

    // 50 hz tick loop (20 ms per tick)
    const auto tick = std::chrono::milliseconds(20);

    while (running)
    {
        // Networking
        if (!server.connected()) {
            server.AcceptConnection();
        }
        else {
            server.HandleConnection();
        }
        std::this_thread::sleep_for(tick);
    }
    // terminate
    return 0;
}
