#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include <zlib.h>

namespace Server::BanchoPacket {
    enum class ParseStatus {
        NeedMoreData,
        Ok,
        Invalid
    };

    struct Packet {
        uint16_t id = 0;
        bool compressed = false;
        std::vector<uint8_t> content;
    };

    constexpr std::size_t modern_header_size = 7;
    constexpr std::size_t legacy_header_size = 6;
    constexpr uint32_t max_content_size = 4 * 1024 * 1024;

    inline uint16_t read_u16_le(const uint8_t* data) {
        return static_cast<uint16_t>(data[0]) | (static_cast<uint16_t>(data[1]) << 8U);
    }

    inline uint32_t read_u32_le(const uint8_t* data) {
        return static_cast<uint32_t>(data[0]) |
            (static_cast<uint32_t>(data[1]) << 8U) |
            (static_cast<uint32_t>(data[2]) << 16U) |
            (static_cast<uint32_t>(data[3]) << 24U);
    }

    inline void append_u16_le(std::vector<uint8_t>& out, uint16_t value) {
        out.push_back(static_cast<uint8_t>(value & 0xFFU));
        out.push_back(static_cast<uint8_t>((value >> 8U) & 0xFFU));
    }

    inline void append_u32_le(std::vector<uint8_t>& out, uint32_t value) {
        out.push_back(static_cast<uint8_t>(value & 0xFFU));
        out.push_back(static_cast<uint8_t>((value >> 8U) & 0xFFU));
        out.push_back(static_cast<uint8_t>((value >> 16U) & 0xFFU));
        out.push_back(static_cast<uint8_t>((value >> 24U) & 0xFFU));
    }

    inline std::optional<std::vector<uint8_t>> gzip_decompress(const uint8_t* data, std::size_t length) {
        z_stream stream{};
        stream.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(data));
        stream.avail_in = static_cast<uInt>(length);
        if (inflateInit2(&stream, 16 + MAX_WBITS) != Z_OK) {
            return std::nullopt;
        }

        std::vector<uint8_t> output;
        output.reserve(length * 2 + 128);
        uint8_t tmp[4096];
        int status = Z_OK;
        while (status == Z_OK) {
            stream.next_out = tmp;
            stream.avail_out = static_cast<uInt>(sizeof(tmp));
            status = inflate(&stream, Z_NO_FLUSH);
            const std::size_t produced = sizeof(tmp) - stream.avail_out;
            output.insert(output.end(), tmp, tmp + produced);
        }

        inflateEnd(&stream);
        if (status != Z_STREAM_END) {
            return std::nullopt;
        }
        return output;
    }

    inline std::optional<std::vector<uint8_t>> gzip_compress(const uint8_t* data, std::size_t length) {
        z_stream stream{};
        stream.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(data));
        stream.avail_in = static_cast<uInt>(length);
        if (deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 16 + MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
            return std::nullopt;
        }

        std::vector<uint8_t> output;
        output.reserve(length / 2 + 64);
        uint8_t tmp[4096];
        int status = Z_OK;
        while (status == Z_OK) {
            stream.next_out = tmp;
            stream.avail_out = static_cast<uInt>(sizeof(tmp));
            status = deflate(&stream, stream.avail_in > 0 ? Z_NO_FLUSH : Z_FINISH);
            const std::size_t produced = sizeof(tmp) - stream.avail_out;
            output.insert(output.end(), tmp, tmp + produced);
        }

        deflateEnd(&stream);
        if (status != Z_STREAM_END) {
            return std::nullopt;
        }
        return output;
    }

    inline ParseStatus try_parse_packet(const std::vector<uint8_t>& stream, Packet& packet, std::size_t& consumed_bytes, bool legacy_header) {
        const std::size_t header_size = legacy_header ? legacy_header_size : modern_header_size;
        if (stream.size() < header_size) {
            consumed_bytes = 0;
            return ParseStatus::NeedMoreData;
        }

        packet.id = read_u16_le(stream.data());
        packet.compressed = legacy_header || stream[2] != 0;
        const uint32_t content_size = read_u32_le(stream.data() + (legacy_header ? 2 : 3));
        if (content_size > max_content_size) {
            consumed_bytes = 0;
            return ParseStatus::Invalid;
        }

        const std::size_t packet_size = header_size + content_size;
        if (stream.size() < packet_size) {
            consumed_bytes = 0;
            return ParseStatus::NeedMoreData;
        }

        const uint8_t* payload = stream.data() + header_size;
        if (!packet.compressed) {
            packet.content.assign(payload, payload + content_size);
            consumed_bytes = packet_size;
            return ParseStatus::Ok;
        }

        const auto decompressed = gzip_decompress(payload, content_size);
        if (!decompressed.has_value()) {
            consumed_bytes = 0;
            return ParseStatus::Invalid;
        }

        packet.content = std::move(decompressed.value());
        consumed_bytes = packet_size;
        return ParseStatus::Ok;
    }

    inline std::optional<std::vector<uint8_t>> serialize_packet(uint16_t packet_id, const std::vector<uint8_t>& content, bool compress, bool legacy_header) {
        std::vector<uint8_t> payload = content;
        if (compress || legacy_header) {
            const auto compressed = gzip_compress(content.data(), content.size());
            if (!compressed.has_value()) {
                return std::nullopt;
            }
            payload = std::move(compressed.value());
        }

        if (payload.size() > max_content_size) {
            return std::nullopt;
        }

        std::vector<uint8_t> out;
        out.reserve((legacy_header ? legacy_header_size : modern_header_size) + payload.size());
        append_u16_le(out, packet_id);
        if (!legacy_header) {
            out.push_back((compress || legacy_header) ? 1U : 0U);
        }
        append_u32_le(out, static_cast<uint32_t>(payload.size()));
        out.insert(out.end(), payload.begin(), payload.end());
        return out;
    }
}
