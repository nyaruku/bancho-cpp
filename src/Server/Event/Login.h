#pragma once
#include <string>
#include "../Types/Http.h"
#include "../Types/Login.h"
#include "../Types/Packets.h"
#include "../Writer.h"
#include "../Logger/Logger.h"

namespace Server::Event {

    inline void send_login_error(std::shared_ptr<Server::Session::State> state,
                                 Types::LoginError err) {
        std::vector<uint8_t> body;
        Types::Packets::login_reply(body, static_cast<int32_t>(err));
        Types::HttpResponse resp;
        resp.headers["cho-token"]    = "fail";
        resp.headers["Content-Type"] = "application/octet-stream";
        resp.body = std::move(body);
        Writer::enqueue(state, resp.serialize());
    }

    inline void Login(std::shared_ptr<Server::Session::State> state, const Types::HttpRequest& req) {
        const std::string body(req.body.begin(), req.body.end());

        Logger::log(Logger::Category::Connect,
            "Login body (", body.size(), " bytes):\n", body);

        const auto login_req = Types::LoginRequest::parse(body);

        if (!login_req) {
            Logger::log(Logger::Category::Error, "Failed to parse login request");
            send_login_error(state, Types::LoginError::ServerError);
            return;
        }

        Logger::log(Logger::Category::Connect,
            "Login: username=", login_req->username,
            " build=", login_req->client_info.build_name,
            " utc=", login_req->client_info.utc_offset,
            " city=", login_req->client_info.display_city,
            " friend_only_dms=", login_req->client_info.friend_only_dms,
            "\n  exe_hash=", login_req->client_info.client_hash.executable_name_hash,
            "\n  interfaces=", login_req->client_info.client_hash.network_interfaces,
            "\n  interfaces_hash=", login_req->client_info.client_hash.network_interfaces_hash,
            "\n  uid1=", login_req->client_info.client_hash.unique_id_1,
            "\n  uid2=", login_req->client_info.client_hash.unique_id_2);

        // TODO: real authentication — for now accept any credentials with a fixed user ID
        constexpr int32_t user_id = 1000;
        const std::string token   = "session-" + std::to_string(user_id);
        state->osu_token          = token;

        // Build the full After Login packet stream
        std::vector<uint8_t> packets;
        namespace P = Types::Packets;

        // 1. Protocol version (must be first)
        P::protocol_version(packets);

        // 2. Login reply with the user ID
        P::login_reply(packets, user_id);

        // 3. Permissions: Player (1) | Supporter (4) = 5
        P::login_permissions(packets, 1 | 4);

        // 4. User presence (the player's own data)
        P::user_presence(packets,
            user_id,
            login_req->username,
            static_cast<uint8_t>(login_req->client_info.utc_offset),
            /*country_id=*/0,
            /*permissions=*/1 | 4,
            /*mode=*/0,      // osu!
            /*longitude=*/0.0f,
            /*latitude=*/0.0f,
            /*rank=*/1);

        // 5. User stats (idle, no beatmap)
        P::user_stats(packets,
            user_id,
            /*status=*/0,    // Idle
            "",              // status_text
            "",              // beatmap_md5
            /*mods=*/0,
            /*mode=*/0,
            /*beatmap_id=*/0,
            /*ranked_score=*/0,
            /*accuracy=*/1.0f,
            /*play_count=*/0,
            /*total_score=*/0,
            /*rank=*/1,
            /*pp=*/0);

        // 6. Friends list (empty for now)
        P::friends_list(packets);

        // 7. Auto-join #osu — required: client won't finish login without it
        P::channel_available_autojoin(packets, "#osu", "General", 1);

        // 8. Confirm #osu join
        P::channel_join_success(packets, "#osu");

        // 9. End of channel list
        P::channel_info_complete(packets);

        // 10. Silence info (not silenced)
        P::silence_info(packets, 0);

        Logger::log(Logger::Category::Connect,
            "Sending After Login sequence (", packets.size(), " bytes) for user '",
            login_req->username, "' id=", user_id);

        Types::HttpResponse resp;
        resp.headers["cho-token"]    = token;
        resp.headers["Content-Type"] = "application/octet-stream";
        resp.body = std::move(packets);
        Writer::enqueue(state, resp.serialize());
    }
}
