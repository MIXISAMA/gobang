#pragma once

#include "pch.h"
#include "game/game_room.h"

#include "net/idtcp.h"

namespace mixi
{
namespace gobang
{

class ServerGameRoom
{

public:

    enum class State
    {
        Loading,
        Done,
        Fail,
    };

    enum class Instruction: u_int16_t
    {
        Join_As_Player = 0x0000,
        Join_As_Onlooker,
    };

    ServerGameRoom(
        u_int16_t room_id,
        const std::string& nickname,
        bool is_player,
        const boost::asio::ip::tcp::endpoint& remote_endpoint
    );
    ~ServerGameRoom();

    State state();

    void request_room();

    const GameRoom* room() const;

private:

    net::IdtcpSocket socket_;
    u_int16_t room_id_;
    const std::string nickname_;
    bool is_player_;
    const boost::asio::ip::tcp::endpoint remote_endpoint_;

    GameRoom* room_;
    std::atomic<State> state_;

    u_int16_t receive_instruction_;
    std::vector<std::byte> receive_data_;

    void start_receive_();

    void handle_connect_(const boost::system::error_code& error);
    void handle_send_(const boost::system::error_code& error);
    void handle_receive_(const boost::system::error_code& error);
    void handle_join_(const boost::system::error_code& error, std::size_t);

    void join_();

};

} // namespace gobang
} // namespace mixi
