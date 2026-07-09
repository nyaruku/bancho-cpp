#pragma once
namespace Server::Event {
    inline void Connect(std::shared_ptr<Server::Session::State> state) {
        std::error_code ec;
        const auto endpoint = state->socket.remote_endpoint(ec);
        if (!ec) {
            std::cout << "Client connected " << endpoint << "\n";
        }
    }
}