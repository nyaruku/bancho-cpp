#pragma once

namespace Server::Writer {

    inline void do_write(std::shared_ptr<Server::Session::State> state) {
        asio::async_write(state->socket, asio::buffer(state->write_queue.front()), [state](std::error_code ec, std::size_t) {
            state->write_queue.pop();
            if (!ec && !state->write_queue.empty()) {
                do_write(state);
            } else {
                state->writing = false;
            }
        });
    }

    inline void enqueue(std::shared_ptr<Server::Session::State> state, std::vector<uint8_t> data) {
        state->write_queue.push(std::move(data));
        if (!state->writing) {
            state->writing = true;
            do_write(state);
        }
    }
}