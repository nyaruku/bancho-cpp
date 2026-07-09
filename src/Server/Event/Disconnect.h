#pragma once
namespace Server::Event {
    inline void Disconnect(std::shared_ptr<Server::Session::State> state) {
        std::error_code ec;
        const auto endpoint = state->socket.remote_endpoint(ec);
        if (!ec) {
            std::cout << "Client disconnected " << endpoint << "\n";
        } else {
            std::cout << "Client disconnected (ERROR)" << "\n";
        }
        state->socket.close(ec);
    }
}