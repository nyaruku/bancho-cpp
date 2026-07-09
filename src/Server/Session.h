#pragma once
#include <iostream>
#include <memory>
#include <utility>
#include <queue>
#include <vector>
#include <asio.hpp>

namespace Server::Session {
    enum {
        max_length = 1024
    };

    struct State {
        asio::ip::tcp::socket socket;
        char read_buf[max_length];
        std::queue<std::vector<uint8_t>> write_queue;
        bool writing = false;

        explicit State(asio::ip::tcp::socket socket) : socket(std::move(socket)) {}
    };
}

#include "Writer.h"
#include "Reader.h"
#include "Event/Connect.h"
#include "Event/Disconnect.h"

namespace Server::Session {
    inline void start(asio::ip::tcp::socket socket) {
        auto state = std::make_shared<State>(std::move(socket));
        Server::Event::Connect(state);
        Server::Reader::do_read(state);
    }
}
