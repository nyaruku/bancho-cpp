#pragma once
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <queue>
#include <vector>
#include <asio.hpp>
#include "BanchoPacket.h"

namespace Server::Session {
    enum {
        max_length = 1024
    };

    struct State {
        asio::ip::tcp::socket socket;
        char read_buf[max_length];

        // HTTP layer
        std::vector<uint8_t> raw_buf;
        bool http_headers_parsed = false;
        std::size_t http_header_end = 0;
        std::size_t http_content_length = 0;
        std::optional<std::string> osu_token;
        std::vector<uint8_t> response_buf; // outgoing bancho packets for the current HTTP response

        // Bancho packet layer
        std::vector<uint8_t> packet_stream;
        std::queue<std::vector<uint8_t>> write_queue;
        std::optional<bool> legacy_header;
        bool writing = false;

        explicit State(asio::ip::tcp::socket socket) : socket(std::move(socket)) {}
    };
}

#include "Writer.h"
#include "Reader.h"
#include "Event/Connect.h"
#include "Event/Disconnect.h"
#include "Event/Packet.h"
#include "Event/Http.h"

namespace Server::Session {
    inline void start(asio::ip::tcp::socket socket) {
        auto state = std::make_shared<State>(std::move(socket));
        Server::Event::Connect(state);
        Server::Reader::do_read(state);
    }
}
