#include <thread>
#include <vector>
#include "Server/Server.h"
#include "Server/Logger/Logger.h"

int main() {
    try
    {
        asio::io_context io_context;
        Server::listen(io_context, BANCHO_HOST, BANCHO_PORT);

        std::vector<std::thread> threads;
        for (int i = 0; i < BANCHO_THREADS; i++) {
            threads.emplace_back([&io_context, i] {
                Server::Logger::log(Server::Logger::Category::Thread, "Thread ", i, " started.");
                io_context.run();
            });
        }
        for (std::thread& thread : threads) {
            thread.join();
        }
    }
    catch (std::exception& e)
    {
        Server::Logger::log(Server::Logger::Category::Error, "Exception: ", e.what());
    }
    return 0;
}