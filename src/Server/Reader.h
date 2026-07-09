#pragma once
#include <algorithm>
#include <iomanip>
#include <sstream>
#include "Logger/Logger.h"

namespace Server::Event {
    inline void Disconnect(std::shared_ptr<Server::Session::State> state);
    inline void Packet(std::shared_ptr<Server::Session::State> state, const Server::BanchoPacket::Packet& packet);
}

namespace Server::Reader {
    inline void log_raw_read(std::shared_ptr<Server::Session::State> state, const std::size_t length) {
        constexpr std::size_t preview_size = 64;
        const std::size_t bytes_to_log = std::min(length, preview_size);

        std::ostringstream hex;
        hex << std::hex << std::setfill('0');
        for (std::size_t i = 0; i < bytes_to_log; i++) {
            if (i != 0) {
                hex << ' ';
            }
            hex << std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(state->read_buf[i]));
        }
        if (length > preview_size) {
            hex << " ...";
        }

        std::error_code ec;
        const auto endpoint = state->socket.remote_endpoint(ec);
        if (!ec) {
            Logger::log(Logger::Category::RawRead, "Raw read ", length, " bytes from ", endpoint, ": ", hex.str());
        } else {
            Logger::log(Logger::Category::RawRead, "Raw read ", length, " bytes: ", hex.str());
        }
    }

    inline Server::BanchoPacket::ParseStatus consume_packet_stream(std::shared_ptr<Server::Session::State> state) {
        Server::BanchoPacket::Packet packet{};
        std::size_t consumed_bytes = 0;
        Server::BanchoPacket::ParseStatus parse_status;

        if (state->legacy_header.has_value()) {
            // Header format locked in after first packet
            parse_status = Server::BanchoPacket::try_parse_packet(state->packet_stream, packet, consumed_bytes, *state->legacy_header);
        } else {
            // Auto-detect on first packet: try modern first, fall back to legacy
            Server::BanchoPacket::Packet modern_packet{};
            std::size_t modern_bytes = 0;
            const BanchoPacket::ParseStatus parseStatus = Server::BanchoPacket::try_parse_packet(state->packet_stream, modern_packet, modern_bytes, false);

            Server::BanchoPacket::Packet legacy_packet{};
            std::size_t legacy_bytes = 0;
            const auto legacy_status = Server::BanchoPacket::try_parse_packet(state->packet_stream, legacy_packet, legacy_bytes, true);

            if (parseStatus == Server::BanchoPacket::ParseStatus::Ok) {
                state->legacy_header = false;
                packet = std::move(modern_packet);
                consumed_bytes = modern_bytes;
                parse_status = parseStatus;
            } else if (legacy_status == Server::BanchoPacket::ParseStatus::Ok) {
                state->legacy_header = true;
                packet = std::move(legacy_packet);
                consumed_bytes = legacy_bytes;
                parse_status = legacy_status;
            } else if (parseStatus == Server::BanchoPacket::ParseStatus::Invalid &&
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

            log_raw_read(state, length);
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