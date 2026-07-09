#pragma once
#include <memory>
#include "../Session.h"
#include "../BanchoPacket.h"
#include "../Types/Serializer.h"

// One file per handler. Each receives the session state and a Reader
// positioned at the start of the packet content.

namespace Server::Packets {
    inline void on_status_change(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_send_message(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_exit(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_request_status(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_pong(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_start_spectating(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_stop_spectating(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_send_frames(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_error_report(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_cant_spectate(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_send_private_message(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_part_lobby(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_join_lobby(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_create_match(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_join_match(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_leave_match(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_match_change_slot(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_match_ready(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_match_lock(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_match_change_settings(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_match_start(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_match_score_update(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_match_complete(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_match_change_beatmap(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_match_change_mods(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_match_load_complete(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_match_no_beatmap(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_match_not_ready(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_match_failed(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_match_has_beatmap(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_match_skip(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_join_channel(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_beatmap_info(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_match_transfer_host(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_add_friend(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_remove_friend(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_match_change_team(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_leave_channel(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_receive_updates(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_set_away_message(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_stats_request(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_match_invite(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_match_change_password(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_tournament_match_info(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_presence_request(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_presence_request_all(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_change_friendonly_dms(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_tournament_join_match_channel(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }

    inline void on_tournament_leave_match_channel(std::shared_ptr<Session::State> state, Types::Serializer::Reader& reader) {
        // TODO
    }
}
