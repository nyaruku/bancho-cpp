#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "PacketId.h"
#include "Serializer.h"
#include "../BanchoPacket.h"

namespace Server::Types {

    struct ClientHash {
        std::string executable_name_hash;
        std::string network_interfaces;
        std::string network_interfaces_hash;
        std::string unique_id_1;
        std::string unique_id_2;
    };

    struct ClientInfo {
        std::string build_name;
        int32_t utc_offset = 0;
        bool display_city = false;
        ClientHash client_hash;
        bool friend_only_dms = false;
    };

    struct LoginRequest {
        std::string username;
        std::string password_md5;
        ClientInfo client_info;

        // Parses the raw HTTP body sent by the osu! client on login.
        // Format: "username\r\npassword_md5\r\nbuild|utc|city|hash|friendonly\r\n"
        static std::optional<LoginRequest> parse(std::string_view body) {
            // Accept both \r\n and \n line endings; also accept missing trailing newline
            auto next_line = [](std::string_view& remaining) -> std::optional<std::string_view> {
                auto pos = remaining.find('\n');
                if (pos == std::string_view::npos) {
                    // Last line with no trailing newline — return what's left if non-empty
                    if (remaining.empty()) return std::nullopt;
                    const auto line = remaining;
                    remaining = {};
                    return line;
                }
                std::string_view line = remaining.substr(0, pos);
                // Strip trailing \r if present
                if (!line.empty() && line.back() == '\r') line.remove_suffix(1);
                remaining.remove_prefix(pos + 1);
                return line;
            };

            auto split_once = [](std::string_view s, char delim, std::string_view& left, std::string_view& right) -> bool {
                const auto pos = s.find(delim);
                if (pos == std::string_view::npos) return false;
                left = s.substr(0, pos);
                right = s.substr(pos + 1);
                return true;
            };

            std::string_view remaining = body;

            const auto username_line = next_line(remaining);
            if (!username_line) return std::nullopt;

            const auto password_line = next_line(remaining);
            if (!password_line) return std::nullopt;

            const auto info_line = next_line(remaining);
            if (!info_line) return std::nullopt;

            // Parse client info: "build|utc|city|hash|friendonly"
            std::string_view info = *info_line;
            std::string_view build_name, rest;
            if (!split_once(info, '|', build_name, rest)) return std::nullopt;

            std::string_view utc_str, rest2;
            if (!split_once(rest, '|', utc_str, rest2)) return std::nullopt;

            std::string_view city_str, rest3;
            if (!split_once(rest2, '|', city_str, rest3)) return std::nullopt;

            std::string_view hash_str, friend_str;
            if (!split_once(rest3, '|', hash_str, friend_str)) return std::nullopt;

            // Strip any trailing whitespace from friend_str
            while (!friend_str.empty() && (friend_str.back() == '\r' || friend_str.back() == '\n' || friend_str.back() == ' '))
                friend_str.remove_suffix(1);

            // Parse client hash: "exe:interfaces:interfaces_hash:uid1:uid2:"
            ClientHash client_hash;
            {
                auto take_segment = [](std::string_view& s) -> std::string_view {
                    const auto pos = s.find(':');
                    if (pos == std::string_view::npos) {
                        const auto seg = s;
                        s = {};
                        return seg;
                    }
                    const auto seg = s.substr(0, pos);
                    s.remove_prefix(pos + 1);
                    return seg;
                };

                client_hash.executable_name_hash   = std::string(take_segment(hash_str));
                client_hash.network_interfaces      = std::string(take_segment(hash_str));
                client_hash.network_interfaces_hash = std::string(take_segment(hash_str));
                client_hash.unique_id_1             = std::string(take_segment(hash_str));
                client_hash.unique_id_2             = std::string(take_segment(hash_str));
            }

            LoginRequest req;
            req.username     = std::string(*username_line);
            req.password_md5 = std::string(*password_line);
            req.client_info.build_name      = std::string(build_name);
            req.client_info.utc_offset      = utc_str.empty() ? 0 : static_cast<int32_t>(std::stoi(std::string(utc_str)));
            req.client_info.display_city    = city_str == "1";
            req.client_info.client_hash     = std::move(client_hash);
            req.client_info.friend_only_dms = friend_str == "1";
            return req;
        }
    };

    enum class LoginError : int32_t {
        NeedsVerification = -8,
        PasswordReset = -7,
        TestBuild = -6,
        ServerError = -5,
        NotActivated = -4,
        Banned = -3,
        NeedsUpdate = -2,
        InvalidCredentials = -1,
    };

    struct LoginReply {
        int32_t user_id; // positive = success, negative = LoginError
        std::string token; // cho-token value; only meaningful on success

        explicit LoginReply(LoginError error)
            : user_id(static_cast<int32_t>(error)) {}

        LoginReply(int32_t id, std::string tok)
            : user_id(id), token(std::move(tok)) {}

        std::optional<std::vector<uint8_t>> serialize(bool legacy_header) const {
            std::vector<uint8_t> content;
            Serializer::write_i32(content, user_id);
            return BanchoPacket::serialize_packet(
                static_cast<uint16_t>(ServerPacketId::LoginReply),
                content, false, legacy_header
            );
        }
    };
}
