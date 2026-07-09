#pragma once
#include <cstdint>
#include <string_view>
#include <vector>
#include "PacketId.h"
#include "Serializer.h"

// Builder functions for Server -> Client bancho packets (modern format).
// Each function appends a complete packet to `out`.
// Packet header: u16 id | u8 compression(0) | u32 content_size | content

namespace Server::Types::Packets {

    inline void append_packet(std::vector<uint8_t>& out,
                              ServerPacketId id,
                              const std::vector<uint8_t>& content) {
        Serializer::write_u16(out, static_cast<uint16_t>(id));
        Serializer::write_u8(out, 0); // no compression
        Serializer::write_u32(out, static_cast<uint32_t>(content.size()));
        out.insert(out.end(), content.begin(), content.end());
    }

    // Packet 75 — ProtocolVersion: i32 version (current = 19)
    inline void protocol_version(std::vector<uint8_t>& out, int32_t version = 19) {
        std::vector<uint8_t> c;
        Serializer::write_i32(c, version);
        append_packet(out, ServerPacketId::ProtocolVersion, c);
    }

    // Packet 5 — LoginReply: i32 user_id (or LoginError negative value)
    inline void login_reply(std::vector<uint8_t>& out, int32_t user_id) {
        std::vector<uint8_t> c;
        Serializer::write_i32(c, user_id);
        append_packet(out, ServerPacketId::LoginReply, c);
    }

    // Packet 71 — LoginPermissions: i32 bitmask (Player=1, Supporter=4, etc.)
    inline void login_permissions(std::vector<uint8_t>& out, int32_t permissions) {
        std::vector<uint8_t> c;
        Serializer::write_i32(c, permissions);
        append_packet(out, ServerPacketId::LoginPermissions, c);
    }

    // Packet 83 — UserPresence (modern format, protocol >= ~15)
    inline void user_presence(std::vector<uint8_t>& out,
                              int32_t user_id,
                              std::string_view username,
                              uint8_t timezone_offset, // UTC offset in hours
                              uint8_t country_id,      // 0 = unknown
                              uint8_t permissions,
                              uint8_t mode,            // 0=osu, 1=taiko, 2=fruits, 3=mania
                              float longitude,
                              float latitude,
                              int32_t rank) {
        std::vector<uint8_t> c;
        Serializer::write_i32(c, user_id);
        Serializer::write_string(c, username);
        Serializer::write_u8(c, static_cast<uint8_t>(timezone_offset + 24));
        Serializer::write_u8(c, country_id);
        Serializer::write_u8(c, static_cast<uint8_t>(permissions | (mode << 5)));
        Serializer::write_f32(c, longitude);
        Serializer::write_f32(c, latitude);
        Serializer::write_i32(c, rank);
        append_packet(out, ServerPacketId::UserPresence, c);
    }

    // Packet 11 — UserStats (modern format, b20250306+: PP is uShort)
    inline void user_stats(std::vector<uint8_t>& out,
                           int32_t user_id,
                           uint8_t status,            // 0=idle, see Status enum
                           std::string_view status_text,
                           std::string_view beatmap_md5,
                           uint32_t mods,
                           uint8_t mode,
                           int32_t beatmap_id,
                           int64_t ranked_score,
                           float accuracy,            // 0.0 – 1.0
                           int32_t play_count,
                           int64_t total_score,
                           int32_t rank,
                           uint16_t pp) {
        std::vector<uint8_t> c;
        Serializer::write_i32(c, user_id);
        // StatusUpdate
        Serializer::write_u8(c, status);
        Serializer::write_string(c, status_text);
        Serializer::write_string(c, beatmap_md5);
        Serializer::write_u32(c, mods);
        Serializer::write_u8(c, mode);
        Serializer::write_i32(c, beatmap_id);
        // Stats
        Serializer::write_i64(c, ranked_score);
        Serializer::write_f32(c, accuracy);
        Serializer::write_i32(c, play_count);
        Serializer::write_i64(c, total_score);
        Serializer::write_i32(c, rank);
        Serializer::write_u16(c, pp);
        append_packet(out, ServerPacketId::UserStats, c);
    }

    // Packet 72 — FriendsList: u16 count + list of i32 user IDs
    inline void friends_list(std::vector<uint8_t>& out,
                             const std::vector<int32_t>& friends = {}) {
        std::vector<uint8_t> c;
        Serializer::write_u16(c, static_cast<uint16_t>(friends.size()));
        for (const int32_t uid : friends)
            Serializer::write_i32(c, uid);
        append_packet(out, ServerPacketId::FriendsList, c);
    }

    // Packet 65 — ChannelAvailable: adds channel to listing (not auto-joined)
    inline void channel_available(std::vector<uint8_t>& out,
                                  std::string_view name,
                                  std::string_view topic,
                                  int16_t user_count) {
        std::vector<uint8_t> c;
        Serializer::write_string(c, name);
        Serializer::write_string(c, topic);
        // write i16 as u16
        Serializer::write_u16(c, static_cast<uint16_t>(user_count));
        append_packet(out, ServerPacketId::ChannelAvailable, c);
    }

    // Packet 67 — ChannelAvailableAutojoin: client auto-joins this channel
    // Note: if the name is "#osu", client will always auto-join it.
    inline void channel_available_autojoin(std::vector<uint8_t>& out,
                                           std::string_view name,
                                           std::string_view topic,
                                           int16_t user_count) {
        std::vector<uint8_t> c;
        Serializer::write_string(c, name);
        Serializer::write_string(c, topic);
        Serializer::write_u16(c, static_cast<uint16_t>(user_count));
        append_packet(out, ServerPacketId::ChannelAvailableAutojoin, c);
    }

    // Packet 64 — ChannelJoinSuccess: confirms the client joined a channel
    inline void channel_join_success(std::vector<uint8_t>& out, std::string_view name) {
        std::vector<uint8_t> c;
        Serializer::write_string(c, name);
        append_packet(out, ServerPacketId::ChannelJoinSuccess, c);
    }

    // Packet 89 — ChannelInfoComplete: signals end of channel list
    inline void channel_info_complete(std::vector<uint8_t>& out) {
        append_packet(out, ServerPacketId::ChannelInfoComplete, {});
    }

    // Packet 92 — SilenceInfo: i32 seconds remaining (0 = not silenced)
    inline void silence_info(std::vector<uint8_t>& out, int32_t seconds = 0) {
        std::vector<uint8_t> c;
        Serializer::write_i32(c, seconds);
        append_packet(out, ServerPacketId::SilenceInfo, c);
    }

    // Packet 24 — Announce: server-side notification message
    inline void announce(std::vector<uint8_t>& out, std::string_view message) {
        std::vector<uint8_t> c;
        Serializer::write_string(c, message);
        append_packet(out, ServerPacketId::Announce, c);
    }

} // namespace Server::Types::Packets
