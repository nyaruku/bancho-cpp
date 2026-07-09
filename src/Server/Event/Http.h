#pragma once
#include "../Types/Http.h"
#include "../Writer.h"
#include "Login.h"

// NOTE: Reader::consume_packet_stream and Event::Disconnect are available here
// because Session.h includes Reader.h and Event/Disconnect.h before this file.

namespace Server::Event {
    inline void Http(std::shared_ptr<Server::Session::State> state) {
        auto req_opt = Types::parse_http_request(
            state->raw_buf, state->http_header_end, state->http_content_length);

        // Consume the processed request bytes regardless of parse result
        const std::size_t consumed = state->http_header_end + state->http_content_length;
        state->raw_buf.erase(state->raw_buf.begin(), state->raw_buf.begin() + consumed);
        state->http_headers_parsed = false;
        state->http_header_end = 0;
        state->http_content_length = 0;

        if (!req_opt) {
            Event::Disconnect(state);
            return;
        }
        const auto& req = *req_opt;

        if (req.is_login()) {
            Event::Login(state, req);
            return;
        }

        // Update session token from request header
        if (const auto token = req.header("osu-token")) {
            state->osu_token = std::string(*token);
        }

        // Parse the HTTP body as a stream of bancho packets
        state->packet_stream = req.body;
        while (true) {
            const auto status = Server::Reader::consume_packet_stream(state);
            if (status == BanchoPacket::ParseStatus::Ok) continue;
            break;
        }
        state->packet_stream.clear();

        // Send accumulated response packets wrapped in HTTP
        Types::HttpResponse resp;
        resp.headers["Content-Type"] = "application/octet-stream";
        resp.headers["Connection"] = "keep-alive";
        if (state->osu_token) resp.headers["cho-token"] = *state->osu_token;
        resp.body = std::move(state->response_buf);
        state->response_buf.clear();
        Writer::enqueue(state, resp.serialize());
    }
}
