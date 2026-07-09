#pragma once
#include "../Logger/Logger.h"

namespace Server::Event {
    inline void Packet(std::shared_ptr<Server::Session::State> state, const Server::BanchoPacket::Packet& packet) {
        std::error_code ec;
        const auto endpoint = state->socket.remote_endpoint(ec);
        if (!ec) {
            Logger::log(Logger::Category::Packet, "Packet ", packet.id, " from ", endpoint, " (", packet.content.size(), " bytes)");
        } else {
            Logger::log(Logger::Category::Packet, "Packet ", packet.id, " (", packet.content.size(), " bytes)");
        }
    }
}
