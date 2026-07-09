#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace Server::Types::Serializer {

    inline void write_u8(std::vector<uint8_t>& out, uint8_t v) {
        out.push_back(v);
    }

    inline void write_u16(std::vector<uint8_t>& out, uint16_t v) {
        out.push_back(static_cast<uint8_t>(v & 0xFF));
        out.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
    }

    inline void write_u32(std::vector<uint8_t>& out, uint32_t v) {
        out.push_back(static_cast<uint8_t>(v & 0xFF));
        out.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
        out.push_back(static_cast<uint8_t>((v >> 16) & 0xFF));
        out.push_back(static_cast<uint8_t>((v >> 24) & 0xFF));
    }

    inline void write_i32(std::vector<uint8_t>& out, int32_t v) {
        write_u32(out, static_cast<uint32_t>(v));
    }

    inline void write_i64(std::vector<uint8_t>& out, int64_t v) {
        const auto u = static_cast<uint64_t>(v);
        out.push_back(static_cast<uint8_t>(u & 0xFF));
        out.push_back(static_cast<uint8_t>((u >> 8) & 0xFF));
        out.push_back(static_cast<uint8_t>((u >> 16) & 0xFF));
        out.push_back(static_cast<uint8_t>((u >> 24) & 0xFF));
        out.push_back(static_cast<uint8_t>((u >> 32) & 0xFF));
        out.push_back(static_cast<uint8_t>((u >> 40) & 0xFF));
        out.push_back(static_cast<uint8_t>((u >> 48) & 0xFF));
        out.push_back(static_cast<uint8_t>((u >> 56) & 0xFF));
    }

    inline void write_f32(std::vector<uint8_t>& out, float v) {
        uint32_t bits;
        static_assert(sizeof(float) == sizeof(uint32_t));
        __builtin_memcpy(&bits, &v, sizeof(bits));
        write_u32(out, bits);
    }

    // osu! string: 0x00 for null/empty, 0x0b + ULEB128(length) + UTF-8 bytes otherwise
    inline void write_string(std::vector<uint8_t>& out, std::string_view s) {
        if (s.empty()) {
            out.push_back(0x00);
            return;
        }
        out.push_back(0x0b);
        uint32_t len = static_cast<uint32_t>(s.size());
        do {
            uint8_t byte = len & 0x7F;
            len >>= 7;
            if (len != 0) byte |= 0x80;
            out.push_back(byte);
        } while (len != 0);
        out.insert(out.end(), s.begin(), s.end());
    }

    struct Reader {
        const uint8_t* data;
        std::size_t size;
        std::size_t pos = 0;
        bool ok = true;

        explicit Reader(const std::vector<uint8_t>& v) : data(v.data()), size(v.size()) {}

        bool at_end() const { return pos >= size; }

        uint8_t read_u8() {
            if (pos >= size) { ok = false; return 0; }
            return data[pos++];
        }

        uint16_t read_u16() {
            if (pos + 2 > size) { ok = false; return 0; }
            const uint16_t v = static_cast<uint16_t>(data[pos]) | (static_cast<uint16_t>(data[pos + 1]) << 8);
            pos += 2;
            return v;
        }

        uint32_t read_u32() {
            if (pos + 4 > size) { ok = false; return 0; }
            const uint32_t v =
                static_cast<uint32_t>(data[pos]) |
                (static_cast<uint32_t>(data[pos + 1]) << 8) |
                (static_cast<uint32_t>(data[pos + 2]) << 16) |
                (static_cast<uint32_t>(data[pos + 3]) << 24);
            pos += 4;
            return v;
        }

        int32_t read_i32() { return static_cast<int32_t>(read_u32()); }

        int64_t read_i64() {
            if (pos + 8 > size) { ok = false; return 0; }
            const uint64_t v =
                static_cast<uint64_t>(data[pos]) |
                (static_cast<uint64_t>(data[pos + 1]) << 8) |
                (static_cast<uint64_t>(data[pos + 2]) << 16) |
                (static_cast<uint64_t>(data[pos + 3]) << 24) |
                (static_cast<uint64_t>(data[pos + 4]) << 32) |
                (static_cast<uint64_t>(data[pos + 5]) << 40) |
                (static_cast<uint64_t>(data[pos + 6]) << 48) |
                (static_cast<uint64_t>(data[pos + 7]) << 56);
            pos += 8;
            return static_cast<int64_t>(v);
        }

        float read_f32() {
            const uint32_t bits = read_u32();
            float v;
            static_assert(sizeof(float) == sizeof(uint32_t));
            __builtin_memcpy(&v, &bits, sizeof(v));
            return v;
        }

        // osu! string: 0x00 = null/empty, 0x0b + ULEB128(length) + UTF-8 bytes
        std::string read_string() {
            const uint8_t marker = read_u8();
            if (!ok || marker == 0x00) return {};
            if (marker != 0x0b) { ok = false; return {}; }

            uint32_t len = 0;
            uint32_t shift = 0;
            while (true) {
                const uint8_t byte = read_u8();
                if (!ok) return {};
                len |= (static_cast<uint32_t>(byte & 0x7F) << shift);
                shift += 7;
                if (!(byte & 0x80)) break;
                if (shift >= 35) { ok = false; return {}; }
            }

            if (pos + len > size) { ok = false; return {}; }
            std::string s(reinterpret_cast<const char*>(data + pos), len);
            pos += len;
            return s;
        }
    };
}
