#pragma once
#include "Session.h"
#include "Logger/Logger.h"

namespace Server {
    inline void do_accept(std::shared_ptr<asio::ip::tcp::acceptor> acceptor) {
        acceptor->async_accept([acceptor](const std::error_code ec, asio::ip::tcp::socket socket) {
            if (!ec) {
                Session::start(std::move(socket));
            }
            do_accept(acceptor);
        });
    }

    inline void listen(asio::io_context& io_context, std::string_view host, short port) {
        Logger::log(Logger::Category::Server, "Bancho listening on ", host, ":", port);
        auto endpoint = asio::ip::tcp::endpoint(asio::ip::make_address(host), port);
        auto acceptor = std::make_shared<asio::ip::tcp::acceptor>(io_context, endpoint);
        do_accept(std::move(acceptor));
    }
}