#pragma once

#include "pch.h"
#include "game/room.h"

#include "net/idtcp_client.h"

namespace mixi
{
namespace gobang
{

class ServerGameRoom
{

public:

    enum class JoinRoomState
    {
        Pending,
        Done,
        Failed,
    };

    ServerGameRoom(boost::asio::io_context& io_context);
    ~ServerGameRoom();

    void join_room(
        const boost::asio::ip::tcp::endpoint& remote_endpoint,
        u_int16_t room_id,
        const std::string& nickname,
        bool is_player
    );

    JoinRoomState join_room_state();

    const game::Room& game_room();

protected:

    enum class C_Instruction: u_int16_t
    {
        Generic_Error_Notification = 0x0000,
        All_Room_Information,
        Join_Room,
    };

    enum class S_Instruction: u_int16_t
    {
        Generic_Error_Notification = 0x0000,
        Join_Room,
    };

    boost::asio::io_context& io_context_;

    std::shared_ptr<net::IdtcpClient> client_;
    std::shared_ptr<game::Room> game_room_;

    const net::IdtcpClient::Distribute distribute_;

    JoinRoomState join_room_state_;

    std::string nickname_;

    void connect_error_notice_(
        bool error,
        u_int16_t room_id,
        const std::string& nickname,
        bool is_player
    );

private:

    void send_join_room_(
        u_int16_t room_id,
        const std::string& nickname,
        bool is_player
    );

    void receive_generic_error_notification_(const std::vector<std::byte>& data);
    void receive_all_room_information_      (const std::vector<std::byte>& data);
    void receive_join_room_                 (const std::vector<std::byte>& data);
    void receive_leave_room_                (const std::vector<std::byte>& data);

};

} // namespace gobang
} // namespace mixi
