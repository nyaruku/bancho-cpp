#include <iostream>
#include <thread>
#include <vector>
#include "Server/Server.h"

int main() {
    try
    {
        asio::io_context io_context;
        Server::listen(io_context, BANCHO_HOST, BANCHO_PORT);

        std::vector<std::thread> threads;
        for (int i = 0; i < BANCHO_THREADS; i++) {
            threads.emplace_back([&io_context, i] {
                std::cout << "Thread " << i << " started.\n";
                io_context.run();
            });
        }
        for (std::thread& thread : threads) {
            thread.join();
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}