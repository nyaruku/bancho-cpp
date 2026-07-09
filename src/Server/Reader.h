#pragma once

namespace Server::Event {
    inline void Disconnect(std::shared_ptr<Server::Session::State> state);
}

namespace Server::Reader {
    inline void do_read(std::shared_ptr<Server::Session::State> state) {
        state->socket.async_read_some(asio::buffer(state->read_buf, Server::Session::max_length), [state](const std::error_code ec, std::size_t length) {
            if (ec) {
                Server::Event::Disconnect(state);
                return;
            }
            // Keep connection open
            do_read(state);
        });
    }
}