#pragma once

#include "pch.h"
#include "game/room.h"

#include "core/lock.h"
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

    boost::asio::io_context& io_context;

    ServerGameRoom(
        boost::asio::io_context& io_context
    );
    ~ServerGameRoom();

    void join_room(
        const boost::asio::ip::tcp::endpoint& remote_endpoint,
        uint8_t room_id,
        const std::string& username,
        const std::string& password,
        char role // P|O
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

    ReadFirstBuffer<game::Room> game_room_;

    // game::Room& game_room_;
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

    uint16_t room_id_;
    std::string username_;
    std::string password_;
    char role_;


    boost::asio::awaitable<void> send_join_room_(const std::vector<std::byte> cipher);
    boost::asio::awaitable<void> send_leave_room_();
    boost::asio::awaitable<void> send_send_message_(const std::string& message);

    void receive_fatal_error_   (const std::vector<std::byte>& data);
    void receive_public_key_    (const std::vector<std::byte>& data);
    void receive_you_join_room_ (const std::vector<std::byte>& data);
    void receive_user_info_     (const std::vector<std::byte>& data);
    void receive_join_room_                 (const std::vector<std::byte>& data);
    void receive_leave_room_                (const std::vector<std::byte>& data);
    void receive_message_                   (const std::vector<std::byte>& data);

};

} // namespace gobang
} // namespace mixi
