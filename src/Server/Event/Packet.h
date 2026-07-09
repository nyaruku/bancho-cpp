#pragma once

namespace Server::Event {
    inline void Packet(std::shared_ptr<Server::Session::State> state, const Server::BanchoPacket::Packet& packet) {
        std::error_code ec;
        const auto endpoint = state->socket.remote_endpoint(ec);
        if (!ec) {
            std::cout << "Packet " << packet.id << " from " << endpoint << " (" << packet.content.size() << " bytes)\n";
        } else {
            std::cout << "Packet " << packet.id << " (" << packet.content.size() << " bytes)\n";
        }
    }
}
