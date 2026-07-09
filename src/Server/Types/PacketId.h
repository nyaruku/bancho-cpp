#pragma once
#include <cstdint>
#include <string_view>

namespace Server::Types {
    // Server -> Client packet IDs (most recent osu! protocol)
    enum class ServerPacketId : uint16_t {
        LoginReply                  = 5,
        CommandError                = 6,
        SendMessage                 = 7,
        Ping                        = 8,
        IrcChangeUsername           = 9,
        IrcQuit                     = 10,
        UserStats                   = 11,
        UserQuit                    = 12,
        SpectatorJoined             = 13,
        SpectatorLeft               = 14,
        SpectateFrames              = 15,
        VersionUpdate               = 19,
        CantSpectate                = 22,
        GetAttention                = 23,
        Announce                    = 24,
        UpdateMatch                 = 26,
        NewMatch                    = 27,
        DisbandMatch                = 28,
        LobbyJoin                   = 34,
        LobbyPart                   = 35,
        MatchJoinSuccess            = 36,
        MatchJoinFail               = 37,
        FellowSpectatorJoined       = 42,
        FellowSpectatorLeft         = 43,
        AllPlayersLoaded            = 45,
        MatchStart                  = 46,
        MatchScoreUpdate            = 48,
        MatchTransferHost           = 50,
        MatchAllPlayersLoaded       = 53,
        MatchPlayerFailed           = 57,
        MatchComplete               = 58,
        MatchSkip                   = 61,
        Unauthorized                = 62,
        ChannelJoinSuccess          = 64,
        ChannelAvailable            = 65,
        ChannelRevoked              = 66,
        ChannelAvailableAutojoin    = 67,
        BeatmapInfoReply            = 69,
        LoginPermissions            = 71,
        FriendsList                 = 72,
        ProtocolVersion             = 75,
        MenuIcon                    = 76,
        Monitor                     = 80,
        MatchPlayerSkipped          = 81,
        UserPresence                = 83,
        IrcOnly                     = 84,
        Restart                     = 86,
        Invite                      = 88,
        ChannelInfoComplete         = 89,
        MatchChangePassword         = 91,
        SilenceInfo                 = 92,
        UserSilenced                = 94,
        UserPresenceSingle          = 95,
        UserPresenceBundle          = 96,
        UserDmBlocked               = 100,
        TargetIsSilenced            = 101,
        VersionUpdateForced         = 102,
        SwitchServer                = 103,
        AccountRestricted           = 104,
        RTX                         = 105,
        MatchAbort                  = 106,
        SwitchTournamentServer      = 107,
    };

    // Client -> Server packet IDs (most recent osu! protocol)
    enum class ClientPacketId : uint16_t {
        StatusChange                = 0,
        SendMessage                 = 1,
        Exit                        = 2,
        RequestStatus               = 3,
        Pong                        = 4,
        StartSpectating             = 16,
        StopSpectating              = 17,
        SendFrames                  = 18,
        ErrorReport                 = 20,
        CantSpectate                = 21,
        SendPrivateMessage          = 25,
        PartLobby                   = 29,
        JoinLobby                   = 30,
        CreateMatch                 = 31,
        JoinMatch                   = 32,
        LeaveMatch                  = 33,
        MatchChangeSlot             = 38,
        MatchReady                  = 39,
        MatchLock                   = 40,
        MatchChangeSettings         = 41,
        MatchStart                  = 44,
        MatchScoreUpdate            = 47,
        MatchComplete               = 49,
        MatchChangeBeatmap          = 50,
        MatchChangeMods             = 51,
        MatchLoadComplete           = 52,
        MatchNoBeatmap              = 54,
        MatchNotReady               = 55,
        MatchFailed                 = 56,
        MatchHasBeatmap             = 59,
        MatchSkip                   = 60,
        JoinChannel                 = 63,
        BeatmapInfo                 = 68,
        MatchTransferHost           = 70,
        AddFriend                   = 73,
        RemoveFriend                = 74,
        MatchChangeTeam             = 77,
        LeaveChannel                = 78,
        ReceiveUpdates              = 79,
        SetAwayMessage              = 82,
        StatsRequest                = 85,
        MatchInvite                 = 87,
        MatchChangePassword         = 90,
        TournamentMatchInfo         = 93,
        PresenceRequest             = 97,
        PresenceRequestAll          = 98,
        ChangeFriendonlyDms         = 99,
        TournamentJoinMatchChannel  = 108,
        TournamentLeaveMatchChannel = 109,
    };

    inline std::string_view client_packet_name(const ClientPacketId id) {
        switch (id) {
            case ClientPacketId::StatusChange:                return "StatusChange";
            case ClientPacketId::SendMessage:                 return "SendMessage";
            case ClientPacketId::Exit:                        return "Exit";
            case ClientPacketId::RequestStatus:               return "RequestStatus";
            case ClientPacketId::Pong:                        return "Pong";
            case ClientPacketId::StartSpectating:             return "StartSpectating";
            case ClientPacketId::StopSpectating:              return "StopSpectating";
            case ClientPacketId::SendFrames:                  return "SendFrames";
            case ClientPacketId::ErrorReport:                 return "ErrorReport";
            case ClientPacketId::CantSpectate:                return "CantSpectate";
            case ClientPacketId::SendPrivateMessage:          return "SendPrivateMessage";
            case ClientPacketId::PartLobby:                   return "PartLobby";
            case ClientPacketId::JoinLobby:                   return "JoinLobby";
            case ClientPacketId::CreateMatch:                 return "CreateMatch";
            case ClientPacketId::JoinMatch:                   return "JoinMatch";
            case ClientPacketId::LeaveMatch:                  return "LeaveMatch";
            case ClientPacketId::MatchChangeSlot:             return "MatchChangeSlot";
            case ClientPacketId::MatchReady:                  return "MatchReady";
            case ClientPacketId::MatchLock:                   return "MatchLock";
            case ClientPacketId::MatchChangeSettings:         return "MatchChangeSettings";
            case ClientPacketId::MatchStart:                  return "MatchStart";
            case ClientPacketId::MatchScoreUpdate:            return "MatchScoreUpdate";
            case ClientPacketId::MatchComplete:               return "MatchComplete";
            case ClientPacketId::MatchChangeBeatmap:          return "MatchChangeBeatmap";
            case ClientPacketId::MatchChangeMods:             return "MatchChangeMods";
            case ClientPacketId::MatchLoadComplete:           return "MatchLoadComplete";
            case ClientPacketId::MatchNoBeatmap:              return "MatchNoBeatmap";
            case ClientPacketId::MatchNotReady:               return "MatchNotReady";
            case ClientPacketId::MatchFailed:                 return "MatchFailed";
            case ClientPacketId::MatchHasBeatmap:             return "MatchHasBeatmap";
            case ClientPacketId::MatchSkip:                   return "MatchSkip";
            case ClientPacketId::JoinChannel:                 return "JoinChannel";
            case ClientPacketId::BeatmapInfo:                 return "BeatmapInfo";
            case ClientPacketId::MatchTransferHost:           return "MatchTransferHost";
            case ClientPacketId::AddFriend:                   return "AddFriend";
            case ClientPacketId::RemoveFriend:                return "RemoveFriend";
            case ClientPacketId::MatchChangeTeam:             return "MatchChangeTeam";
            case ClientPacketId::LeaveChannel:                return "LeaveChannel";
            case ClientPacketId::ReceiveUpdates:              return "ReceiveUpdates";
            case ClientPacketId::SetAwayMessage:              return "SetAwayMessage";
            case ClientPacketId::StatsRequest:                return "StatsRequest";
            case ClientPacketId::MatchInvite:                 return "MatchInvite";
            case ClientPacketId::MatchChangePassword:         return "MatchChangePassword";
            case ClientPacketId::TournamentMatchInfo:         return "TournamentMatchInfo";
            case ClientPacketId::PresenceRequest:             return "PresenceRequest";
            case ClientPacketId::PresenceRequestAll:          return "PresenceRequestAll";
            case ClientPacketId::ChangeFriendonlyDms:         return "ChangeFriendonlyDms";
            case ClientPacketId::TournamentJoinMatchChannel:  return "TournamentJoinMatchChannel";
            case ClientPacketId::TournamentLeaveMatchChannel: return "TournamentLeaveMatchChannel";
            default:                                          return "Unknown";
        }
    }
}

