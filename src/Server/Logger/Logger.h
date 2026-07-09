#pragma once
#include <iostream>
#include <mutex>
#include "Config.h"

namespace Server::Logger {
    enum class Category {
        Server,
        Thread,
        Connect,
        Disconnect,
        Packet,
        RawRead,
        Error
    };

    inline constexpr bool is_enabled(const Category category) {
        switch (category) {
            case Category::Server:
                return Config::server;
            case Category::Thread:
                return Config::thread;
            case Category::Connect:
                return Config::connect;
            case Category::Disconnect:
                return Config::disconnect;
            case Category::Packet:
                return Config::packet;
            case Category::RawRead:
                return Config::raw_read;
            case Category::Error:
                return Config::error;
        }
        return false;
    }

    template <typename... Args>
    inline void log(const Category category, Args&&... args) {
        if (!is_enabled(category)) {
            return;
        }

        static std::mutex mutex;
        std::lock_guard lock(mutex);
        auto& stream = category == Category::Error ? std::cerr : std::cout;
        (stream << ... << args) << "\n";
    }
}