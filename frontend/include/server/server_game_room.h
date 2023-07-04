#pragma once

#include "pch.h"
#include "game/room.h"

#include "net/idtcp.h"

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

    ServerGameRoom(
        boost::asio::io_context& io_context,
        game::Room& game_room
    );
    ~ServerGameRoom();

    void join_room(
        const boost::asio::ip::tcp::endpoint& remote_endpoint,
        u_int16_t room_id,
        const std::string& nickname,
        bool is_player
    );

    JoinRoomState join_room_state();

    void leave_room();

    void send_message(const std::string& message);
    bool has_message();
    std::tuple<std::string, time_t, std::string> pop_message();

protected:

    enum class C_Instruction: u_int16_t
    {
        Generic_Error_Notification = 0x0000,
        All_Room_Information,
        Join_Room,
        Leave_Room,
        Message,
    };

    enum class S_Instruction: u_int16_t
    {
        Generic_Error_Notification = 0x0000,
        All_Room_Information,
        Join_Room,
        Leave_Room,
        Message,
    };

    boost::asio::io_context& io_context_;

    game::Room& game_room_;
    net::IdtcpClient client_;
    const net::IdtcpClient::Distribute distribute_;

    JoinRoomState join_room_state_;

    std::shared_mutex message_queue_mutex_;
    std::queue<std::tuple<
        std::string,
        time_t,
        std::string
    >> message_queue_;

private:

    boost::asio::awaitable<void> connect_and_join_room_(
        const boost::asio::ip::tcp::endpoint& remote_endpoint,
        u_int16_t room_id,
        const std::string& nickname,
        bool is_player
    );
    boost::asio::awaitable<void> send_leave_room_();
    boost::asio::awaitable<void> send_send_message_(const std::string& message);

    void receive_generic_error_notification_(const std::vector<std::byte>& data);
    void receive_all_room_information_      (const std::vector<std::byte>& data);
    void receive_join_room_                 (const std::vector<std::byte>& data);
    void receive_leave_room_                (const std::vector<std::byte>& data);
    void receive_message_                   (const std::vector<std::byte>& data);

};

} // namespace gobang
} // namespace mixi
