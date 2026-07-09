#pragma once

namespace Server::Logger::Config {
    inline constexpr bool server = true;
    inline constexpr bool thread = true;
    inline constexpr bool connect = true;
    inline constexpr bool disconnect = true;
    inline constexpr bool packet = true;
    inline constexpr bool raw_read = true;
    inline constexpr bool error = true;
}
