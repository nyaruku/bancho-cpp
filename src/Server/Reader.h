#pragma once

namespace Server::Event {
    inline void Disconnect(std::shared_ptr<Server::Session::State> state);
    inline void Packet(std::shared_ptr<Server::Session::State> state, const Server::BanchoPacket::Packet& packet);
}

namespace Server::Reader {
    inline Server::BanchoPacket::ParseStatus consume_packet_stream(std::shared_ptr<Server::Session::State> state) {
        Server::BanchoPacket::Packet packet{};
        std::size_t consumed_bytes = 0;
        Server::BanchoPacket::ParseStatus parse_status = Server::BanchoPacket::ParseStatus::NeedMoreData;

        if (BANCHO_LEGACY_HEADER > 0 || state->legacy_header == true) {
            state->legacy_header = true;
            parse_status = Server::BanchoPacket::try_parse_packet(state->packet_stream, packet, consumed_bytes, true);
        } else if (BANCHO_LEGACY_HEADER == 0 || state->legacy_header == false) {
            state->legacy_header = false;
            parse_status = Server::BanchoPacket::try_parse_packet(state->packet_stream, packet, consumed_bytes, false);
        } else {
            Server::BanchoPacket::Packet modern_packet{};
            std::size_t modern_consumed_bytes = 0;
            const auto modern_status = Server::BanchoPacket::try_parse_packet(state->packet_stream, modern_packet, modern_consumed_bytes, false);

            Server::BanchoPacket::Packet legacy_packet{};
            std::size_t legacy_consumed_bytes = 0;
            const auto legacy_status = Server::BanchoPacket::try_parse_packet(state->packet_stream, legacy_packet, legacy_consumed_bytes, true);

            if (modern_status == Server::BanchoPacket::ParseStatus::Ok) {
                state->legacy_header = false;
                packet = std::move(modern_packet);
                consumed_bytes = modern_consumed_bytes;
                parse_status = modern_status;
            } else if (legacy_status == Server::BanchoPacket::ParseStatus::Ok) {
                state->legacy_header = true;
                packet = std::move(legacy_packet);
                consumed_bytes = legacy_consumed_bytes;
                parse_status = legacy_status;
            } else if (modern_status == Server::BanchoPacket::ParseStatus::Invalid &&
                       legacy_status == Server::BanchoPacket::ParseStatus::Invalid) {
                parse_status = Server::BanchoPacket::ParseStatus::Invalid;
            } else {
                parse_status = Server::BanchoPacket::ParseStatus::NeedMoreData;
            }
        }

        if (parse_status != Server::BanchoPacket::ParseStatus::Ok) {
            return parse_status;
        }

        if (consumed_bytes == 0 || consumed_bytes > state->packet_stream.size()) {
            return Server::BanchoPacket::ParseStatus::Invalid;
        }

        state->packet_stream.erase(state->packet_stream.begin(), state->packet_stream.begin() + consumed_bytes);
        Server::Event::Packet(state, packet);
        return Server::BanchoPacket::ParseStatus::Ok;
    }

    inline void do_read(std::shared_ptr<Server::Session::State> state) {
        state->socket.async_read_some(asio::buffer(state->read_buf, Server::Session::max_length), [state](const std::error_code ec, std::size_t length) {
            if (ec) {
                Server::Event::Disconnect(state);
                return;
            }

            state->packet_stream.insert(state->packet_stream.end(), state->read_buf, state->read_buf + length);
            while (true) {
                const auto parse_status = consume_packet_stream(state);
                if (parse_status == Server::BanchoPacket::ParseStatus::Ok) {
                    continue;
                }
                if (parse_status == Server::BanchoPacket::ParseStatus::Invalid) {
                    Server::Event::Disconnect(state);
                    return;
                }
                break;
            }

            // Keep connection open
            do_read(state);
        });
    }
}