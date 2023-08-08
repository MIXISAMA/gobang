#pragma once

#include "pch.h"
#include "game/user.h"
#include "game/room.h"

#include "mixi/core/lock.h"
#include "mixi/net/idtcp.h"

namespace mixi
{
namespace gobang
{

using msg_t = std::tuple<std::string, time_t, std::string>;

class ServerGameRoom
{

public:

    enum class JoinRoomState
    {
        JOINING,
        JOINED,
        FAILED,
    };

    boost::asio::io_context& io_context;

    ServerGameRoom(
        boost::asio::io_context& io_context
    );
    ~ServerGameRoom();

    void connect_and_send_join_room(
        const boost::asio::ip::tcp::endpoint& remote_endpoint,
        uint8_t room_id,
        const std::string& username,
        const std::string& password,
        char role // P|O
    );

    void send_user_info(const std::string& username) const;
    void send_leave_room() const;
    void send_player_ready() const;
    void send_player_stone(std::byte coor) const;
    void send_player_regret() const;
    void send_agree_regret(bool agree) const;
    void send_player_tie() const;
    void send_agree_tie(bool agree) const;
    void send_give_up() const;
    void send_message(const std::string& message);

    boost::signals2::connection
    connect_join_room(const std::function<void(JoinRoomState)>& callback);
    boost::signals2::connection
    connect_agree_regret(const std::function<void(bool)>& callback);
    boost::signals2::connection
    connect_agree_tie(const std::function<void(bool)>& callback);

    ReadFirstBuffer<game::Room>& room();
    ReadTryQueue<game::User>& users();
    ReadTryQueue<msg_t>& messages();

    const std::string& username() const;

private:

    enum class C_Instruction: uint16_t
    {
        FATAL_ERROR = 0x0000,
        PUBLIC_KEY,
        YOU_JOIN_ROOM,
        USER_INFO,
        OTHER_JOIN_ROOM,
        USER_LEAVE,
        PLAYER_READY,
        GAME_START,
        PLAYER_STONE,
        PLAYER_REGRET,
        AGREE_REGRET,
        PLAYER_TIE,
        AGREE_TIE,
        GAME_OVER,
        MESSAGE,
    };

    enum class S_Instruction: uint16_t
    {
        FATAL_ERROR = 0x0000,
        USER_JOIN_ROOM,
        USER_INFO,
        USER_LEAVE,
        PLAYER_READY,
        PLAYER_STONE,
        PLAYER_REGRET,
        AGREE_REGRET,
        PLAYER_TIE,
        AGREE_TIE,
        GIVE_UP,
        MESSAGE,
    };

    ReadFirstBuffer<game::Room> game_room_;
    ReadTryQueue<game::User> users_;
    ReadTryQueue<msg_t> messages_;
    boost::signals2::signal<void(JoinRoomState)> signal_join_room_;
    boost::signals2::signal<void(bool)> signal_agree_regret_;
    boost::signals2::signal<void(bool)> signal_agree_tie_;

    net::IdtcpClient client_;
    const net::IdtcpClient::Distribute distribute_;

    uint8_t room_id_;
    std::string username_;
    std::string password_;
    char role_;

    void on_connected_();
    void on_disconnected_();

    void receive_fatal_error_    (const std::vector<std::byte>& data);
    void receive_public_key_     (const std::vector<std::byte>& data);
    void receive_you_join_room_  (const std::vector<std::byte>& data);
    void receive_user_info_      (const std::vector<std::byte>& data);
    void receive_other_join_room_(const std::vector<std::byte>& data);
    void receive_leave_room_     (const std::vector<std::byte>& data);
    void receive_player_ready_   (const std::vector<std::byte>& data);
    void receive_game_start_     (const std::vector<std::byte>& data);
    void receive_player_stone_   (const std::vector<std::byte>& data);
    void receive_player_regret_  (const std::vector<std::byte>& data);
    void receive_agree_regret_   (const std::vector<std::byte>& data);
    void receive_player_tie_     (const std::vector<std::byte>& data);
    void receive_agree_tie_      (const std::vector<std::byte>& data);
    void receive_game_over_      (const std::vector<std::byte>& data);
    void receive_message_        (const std::vector<std::byte>& data);

};

} // namespace gobang
} // namespace mixi
