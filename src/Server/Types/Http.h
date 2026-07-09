#pragma once
#include <algorithm>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace Server::Types {

struct HttpRequest {
    std::string method;
    std::string path;
    std::unordered_map<std::string, std::string> headers; // lowercase keys
    std::vector<uint8_t> body;

    std::optional<std::string_view> header(std::string_view name) const {
        std::string key(name);
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        const auto it = headers.find(key);
        if (it == headers.end()) return std::nullopt;
        return std::string_view(it->second);
    }

    bool is_login() const { return !header("osu-token").has_value(); }
};

struct HttpResponse {
    int status = 200;
    std::string status_text = "OK";
    std::unordered_map<std::string, std::string> headers;
    std::vector<uint8_t> body;

    std::vector<uint8_t> serialize() const {
        std::ostringstream ss;
        ss << "HTTP/1.1 " << status << " " << status_text << "\r\n";
        ss << "Content-Length: " << body.size() << "\r\n";
        for (const auto& [k, v] : headers) {
            ss << k << ": " << v << "\r\n";
        }
        ss << "\r\n";
        const std::string hdr = ss.str();
        std::vector<uint8_t> out;
        out.reserve(hdr.size() + body.size());
        out.insert(out.end(), hdr.begin(), hdr.end());
        out.insert(out.end(), body.begin(), body.end());
        return out;
    }
};

// Returns the offset of the first body byte (position after \r\n\r\n), or nullopt.
inline std::optional<std::size_t> find_http_header_end(const std::vector<uint8_t>& buf) {
    if (buf.size() < 4) return std::nullopt;
    for (std::size_t i = 0; i <= buf.size() - 4; i++) {
        if (buf[i] == '\r' && buf[i+1] == '\n' && buf[i+2] == '\r' && buf[i+3] == '\n') {
            return i + 4;
        }
    }
    return std::nullopt;
}

// Parse a complete HTTP request. header_end = first byte of body, content_length = body size.
inline std::optional<HttpRequest> parse_http_request(
    const std::vector<uint8_t>& buf, std::size_t header_end, std::size_t content_length)
{
    if (buf.size() < header_end + content_length) return std::nullopt;
    const char* raw = reinterpret_cast<const char*>(buf.data());
    std::string_view sv(raw, header_end);

    HttpRequest req;

    // Request line
    const auto first_crlf = sv.find("\r\n");
    if (first_crlf == std::string_view::npos) return std::nullopt;
    const std::string_view request_line = sv.substr(0, first_crlf);
    const auto sp1 = request_line.find(' ');
    if (sp1 == std::string_view::npos) return std::nullopt;
    req.method = std::string(request_line.substr(0, sp1));
    const auto sp2 = request_line.find(' ', sp1 + 1);
    if (sp2 == std::string_view::npos) return std::nullopt;
    req.path = std::string(request_line.substr(sp1 + 1, sp2 - sp1 - 1));

    // Headers
    std::string_view headers_sv = sv.substr(first_crlf + 2);
    while (!headers_sv.empty()) {
        const auto crlf = headers_sv.find("\r\n");
        const std::string_view line = crlf == std::string_view::npos ? headers_sv : headers_sv.substr(0, crlf);
        if (!line.empty()) {
            const auto colon = line.find(':');
            if (colon != std::string_view::npos) {
                std::string key(line.substr(0, colon));
                std::transform(key.begin(), key.end(), key.begin(), ::tolower);
                std::string_view val = line.substr(colon + 1);
                while (!val.empty() && val.front() == ' ') val.remove_prefix(1);
                req.headers[std::move(key)] = std::string(val);
            }
        }
        if (crlf == std::string_view::npos) break;
        headers_sv.remove_prefix(crlf + 2);
    }

    req.body.assign(buf.begin() + header_end, buf.begin() + header_end + content_length);
    return req;
}

} // namespace Server::Types
