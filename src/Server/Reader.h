#pragma once
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include "Logger/Logger.h"
#include "Types/Http.h"

namespace Server::Event {
    inline void Disconnect(std::shared_ptr<Server::Session::State> state);
    inline void Packet(std::shared_ptr<Server::Session::State> state, const Server::BanchoPacket::Packet& packet);
    inline void Http(std::shared_ptr<Server::Session::State> state);
}

namespace Server::Reader {
    inline void log_raw_read(std::shared_ptr<Server::Session::State> state, const std::size_t length) {
        constexpr std::size_t preview_size = 64;
        const std::size_t bytes_to_log = std::min(length, preview_size);

        std::ostringstream hex;
        hex << std::hex << std::setfill('0');
        for (std::size_t i = 0; i < bytes_to_log; i++) {
            if (i != 0) hex << ' ';
            hex << std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(state->read_buf[i]));
        }
        if (length > preview_size) hex << " ...";

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
            parse_status = Server::BanchoPacket::try_parse_packet(state->packet_stream, packet, consumed_bytes, *state->legacy_header);
        } else {
            // Auto-detect on first packet: try modern first, fall back to legacy
            Server::BanchoPacket::Packet modern_packet{};
            std::size_t modern_bytes = 0;
            const auto modern_status = Server::BanchoPacket::try_parse_packet(state->packet_stream, modern_packet, modern_bytes, false);

            Server::BanchoPacket::Packet legacy_packet{};
            std::size_t legacy_bytes = 0;
            const auto legacy_status = Server::BanchoPacket::try_parse_packet(state->packet_stream, legacy_packet, legacy_bytes, true);

            if (modern_status == Server::BanchoPacket::ParseStatus::Ok) {
                state->legacy_header = false;
                packet = std::move(modern_packet);
                consumed_bytes = modern_bytes;
                parse_status = modern_status;
            } else if (legacy_status == Server::BanchoPacket::ParseStatus::Ok) {
                state->legacy_header = true;
                packet = std::move(legacy_packet);
                consumed_bytes = legacy_bytes;
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
        state->socket.async_read_some(asio::buffer(state->read_buf, Server::Session::max_length),
            [state](const std::error_code ec, std::size_t length) {
                if (ec) {
                    Server::Event::Disconnect(state);
                    return;
                }

                log_raw_read(state, length);
                state->raw_buf.insert(state->raw_buf.end(), state->read_buf, state->read_buf + length);

                // Guard against unbounded header accumulation
                constexpr std::size_t max_header_buf = 64 * 1024;
                if (!state->http_headers_parsed && state->raw_buf.size() > max_header_buf) {
                    Server::Event::Disconnect(state);
                    return;
                }

                // Try to find the end of HTTP headers
                if (!state->http_headers_parsed) {
                    if (const auto pos = Types::find_http_header_end(state->raw_buf)) {
                        state->http_header_end = *pos;
                        state->http_headers_parsed = true;

                        // Extract Content-Length
                        const char* raw = reinterpret_cast<const char*>(state->raw_buf.data());
                        std::string header_section(raw, *pos);
                        std::transform(header_section.begin(), header_section.end(), header_section.begin(), ::tolower);
                        const std::string cl_key = "content-length: ";
                        const auto cl_pos = header_section.find(cl_key);
                        if (cl_pos != std::string::npos) {
                            const auto val_start = cl_pos + cl_key.size();
                            const auto val_end = header_section.find("\r\n", val_start);
                            const auto val = header_section.substr(val_start,
                                val_end != std::string::npos ? val_end - val_start : std::string::npos);
                            try { state->http_content_length = std::stoul(val); } catch (...) {}
                        }
                    }
                }

                // Dispatch once we have the full request (headers + body)
                if (state->http_headers_parsed &&
                    state->raw_buf.size() >= state->http_header_end + state->http_content_length) {
                    Server::Event::Http(state);
                }

                do_read(state);
            });
    }
}