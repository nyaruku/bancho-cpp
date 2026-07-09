#pragma once
#include <iomanip>
#include <sstream>
#include "../Logger/Logger.h"
#include "../Types/PacketId.h"
#include "../Types/Serializer.h"
#include "../Packets/Handlers.h"

namespace Server::Event {
    inline std::string hex_dump(const std::vector<uint8_t>& data) {
        std::ostringstream ss;
        ss << std::hex << std::setfill('0');
        for (std::size_t i = 0; i < data.size(); i++) {
            if (i != 0) ss << ' ';
            ss << std::setw(2) << static_cast<unsigned int>(data[i]);
        }
        return ss.str();
    }

    inline void Packet(std::shared_ptr<Server::Session::State> state, const Server::BanchoPacket::Packet& packet) {
        std::error_code ec;
        const auto endpoint = state->socket.remote_endpoint(ec);

        const auto id = static_cast<Types::ClientPacketId>(packet.id);

        Logger::log(Logger::Category::Packet,
            "Packet ", Types::client_packet_name(id), " (", packet.id, ")",
            " compressed=", packet.compressed,
            " size=", packet.content.size(),
            (!ec ? (std::string(" from ") + endpoint.address().to_string() + ":" + std::to_string(endpoint.port())) : std::string{}),
            "\n  content: ", hex_dump(packet.content));
        Types::Serializer::Reader reader(packet.content);

        switch (id) {
            case Types::ClientPacketId::StatusChange:            return Packets::on_status_change(state, reader);
            case Types::ClientPacketId::SendMessage:             return Packets::on_send_message(state, reader);
            case Types::ClientPacketId::Exit:                    return Packets::on_exit(state, reader);
            case Types::ClientPacketId::RequestStatus:           return Packets::on_request_status(state, reader);
            case Types::ClientPacketId::Pong:                    return Packets::on_pong(state, reader);
            case Types::ClientPacketId::StartSpectating:         return Packets::on_start_spectating(state, reader);
            case Types::ClientPacketId::StopSpectating:          return Packets::on_stop_spectating(state, reader);
            case Types::ClientPacketId::SendFrames:              return Packets::on_send_frames(state, reader);
            case Types::ClientPacketId::ErrorReport:             return Packets::on_error_report(state, reader);
            case Types::ClientPacketId::CantSpectate:            return Packets::on_cant_spectate(state, reader);
            case Types::ClientPacketId::SendPrivateMessage:      return Packets::on_send_private_message(state, reader);
            case Types::ClientPacketId::PartLobby:               return Packets::on_part_lobby(state, reader);
            case Types::ClientPacketId::JoinLobby:               return Packets::on_join_lobby(state, reader);
            case Types::ClientPacketId::CreateMatch:             return Packets::on_create_match(state, reader);
            case Types::ClientPacketId::JoinMatch:               return Packets::on_join_match(state, reader);
            case Types::ClientPacketId::LeaveMatch:              return Packets::on_leave_match(state, reader);
            case Types::ClientPacketId::MatchChangeSlot:         return Packets::on_match_change_slot(state, reader);
            case Types::ClientPacketId::MatchReady:              return Packets::on_match_ready(state, reader);
            case Types::ClientPacketId::MatchLock:               return Packets::on_match_lock(state, reader);
            case Types::ClientPacketId::MatchChangeSettings:     return Packets::on_match_change_settings(state, reader);
            case Types::ClientPacketId::MatchStart:              return Packets::on_match_start(state, reader);
            case Types::ClientPacketId::MatchScoreUpdate:        return Packets::on_match_score_update(state, reader);
            case Types::ClientPacketId::MatchComplete:           return Packets::on_match_complete(state, reader);
            case Types::ClientPacketId::MatchChangeBeatmap:      return Packets::on_match_change_beatmap(state, reader);
            case Types::ClientPacketId::MatchChangeMods:         return Packets::on_match_change_mods(state, reader);
            case Types::ClientPacketId::MatchLoadComplete:       return Packets::on_match_load_complete(state, reader);
            case Types::ClientPacketId::MatchNoBeatmap:          return Packets::on_match_no_beatmap(state, reader);
            case Types::ClientPacketId::MatchNotReady:           return Packets::on_match_not_ready(state, reader);
            case Types::ClientPacketId::MatchFailed:             return Packets::on_match_failed(state, reader);
            case Types::ClientPacketId::MatchHasBeatmap:         return Packets::on_match_has_beatmap(state, reader);
            case Types::ClientPacketId::MatchSkip:               return Packets::on_match_skip(state, reader);
            case Types::ClientPacketId::JoinChannel:             return Packets::on_join_channel(state, reader);
            case Types::ClientPacketId::BeatmapInfo:             return Packets::on_beatmap_info(state, reader);
            case Types::ClientPacketId::MatchTransferHost:       return Packets::on_match_transfer_host(state, reader);
            case Types::ClientPacketId::AddFriend:               return Packets::on_add_friend(state, reader);
            case Types::ClientPacketId::RemoveFriend:            return Packets::on_remove_friend(state, reader);
            case Types::ClientPacketId::MatchChangeTeam:         return Packets::on_match_change_team(state, reader);
            case Types::ClientPacketId::LeaveChannel:            return Packets::on_leave_channel(state, reader);
            case Types::ClientPacketId::ReceiveUpdates:          return Packets::on_receive_updates(state, reader);
            case Types::ClientPacketId::SetAwayMessage:          return Packets::on_set_away_message(state, reader);
            case Types::ClientPacketId::StatsRequest:            return Packets::on_stats_request(state, reader);
            case Types::ClientPacketId::MatchInvite:             return Packets::on_match_invite(state, reader);
            case Types::ClientPacketId::MatchChangePassword:     return Packets::on_match_change_password(state, reader);
            case Types::ClientPacketId::TournamentMatchInfo:     return Packets::on_tournament_match_info(state, reader);
            case Types::ClientPacketId::PresenceRequest:         return Packets::on_presence_request(state, reader);
            case Types::ClientPacketId::PresenceRequestAll:      return Packets::on_presence_request_all(state, reader);
            case Types::ClientPacketId::ChangeFriendonlyDms:     return Packets::on_change_friendonly_dms(state, reader);
            case Types::ClientPacketId::TournamentJoinMatchChannel:  return Packets::on_tournament_join_match_channel(state, reader);
            case Types::ClientPacketId::TournamentLeaveMatchChannel: return Packets::on_tournament_leave_match_channel(state, reader);
            default:
                Logger::log(Logger::Category::Error, "Unknown packet id ", packet.id);
        }
    }
}
