#include "net/server_game_room.h"

#include "core/io_context.h"

namespace mixi
{
namespace gobang
{

ServerGameRoom::ServerGameRoom(
    const std::string& player_name,
    const boost::asio::ip::tcp::endpoint& remote_endpoint
) :
    room_(nullptr),
    player_name_(player_name),
    remote_endpoint_(remote_endpoint),
    socket_(*Io_Context, boost::asio::ip::tcp::endpoint())
{
    // socket_.async_connect(remote_endpoint_, &handle_connect_);
}

const GameRoom* ServerGameRoom::room() const
{
    return room_;
}

void ServerGameRoom::request_room()
{
    // socket_.async_send(remote_endpoint_,
    //     boost::bind(&ServerGameRoom::handle_send_, this)
    // );
}

void ServerGameRoom::handle_connect_()
{

}

void ServerGameRoom::handle_send_()
{

}

void ServerGameRoom::handle_recieve_()
{

}

} // namespace gobang
} // namespace mixi