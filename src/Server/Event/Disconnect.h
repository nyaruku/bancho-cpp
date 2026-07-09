#pragma once
#include "../Logger/Logger.h"

namespace Server::Event {
    inline void Disconnect(std::shared_ptr<Server::Session::State> state) {
        std::error_code ec;
        const auto endpoint = state->socket.remote_endpoint(ec);
        if (!ec) {
            Logger::log(Logger::Category::Disconnect, "Client disconnected ", endpoint);
        } else {
            Logger::log(Logger::Category::Disconnect, "Client disconnected (ERROR)");
        }
        std::error_code close_ec;
        state->socket.shutdown(asio::ip::tcp::socket::shutdown_both, close_ec);
        state->socket.close(close_ec);
    }
}