#include "net/server_game_room.h"

#include "core/io_context.h"
#include "core/log.h"
#include "net/serializer.h"

namespace mixi
{
namespace gobang
{

ServerGameRoom::ServerGameRoom() :
    join_room_state_(JoinRoomState::Pending),
    distribute_{
        std::bind(&ServerGameRoom::receive_generic_error_notification_, this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_all_room_information_,       this, std::placeholders::_1),
    }
{

}

ServerGameRoom::~ServerGameRoom()
{

}

void ServerGameRoom::join_room(
    const boost::asio::ip::tcp::endpoint& remote_endpoint,
    u_int16_t room_id,
    const std::string& nickname,
    bool is_player
) {
    join_room_state_ = JoinRoomState::Pending;
    client_ = std::shared_ptr<net::IdtcpClient>(
        new net::IdtcpClient(
            remote_endpoint,
            std::bind(
                &ServerGameRoom::connect_error_notice_, this,
                std::placeholders::_1,
                room_id, nickname, is_player
            ),
            distribute_
        )
    );
}

ServerGameRoom::JoinRoomState ServerGameRoom::join_room_state()
{
    return join_room_state_;
}

void ServerGameRoom::connect_error_notice_(
    bool error,
    u_int16_t room_id,
    const std::string& nickname,
    bool is_player
) {
    if (error) {
        join_room_state_ = JoinRoomState::Failed;
        return;
    }
    send_join_room_(room_id, nickname, is_player);
}

void ServerGameRoom::send_join_room_(
    u_int16_t room_id,
    const std::string& nickname,
    bool is_player
) {
    Serializer s;
    s << room_id << nickname << is_player;
    client_->send((u_int16_t)S_Instruction::Join_Room, s.raw);
}

void ServerGameRoom::receive_generic_error_notification_(const std::vector<std::byte>& data)
{

}

void ServerGameRoom::receive_all_room_information_      (const std::vector<std::byte>& data)
{
    // Todo
    join_room_state_ = JoinRoomState::Done;
}

} // namespace gobang
} // namespace mixi