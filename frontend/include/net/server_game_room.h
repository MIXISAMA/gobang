#pragma once

#include "pch.h"
#include "game/game_room.h"

namespace mixi
{
namespace gobang
{

class ServerGameRoom
{

public:

    ServerGameRoom(
        const std::string& player_name,
        const boost::asio::ip::tcp::endpoint& remote_endpoint
    );
    ~ServerGameRoom();

    void request_room();

    const GameRoom* room() const;

private:

    GameRoom* room_;

    boost::asio::ip::tcp::socket socket_;
    const std::string player_name_;
    const boost::asio::ip::tcp::endpoint remote_endpoint_;

    void handle_connect_();

    void handle_send_();

    void handle_recieve_();

};

} // namespace gobang
} // namespace mixi
