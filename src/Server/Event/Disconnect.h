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
        state->socket.close(ec);
    }
}