#include "server/server_game_room.h"

#include "core/log.h"
#include "net/serializer.h"

namespace mixi
{
namespace gobang
{

ServerGameRoom::ServerGameRoom(boost::asio::io_context& io_context) :
    io_context_(io_context),
    game_room_(new game::Room("Unknown Room", 16)),
    join_room_state_(JoinRoomState::Pending),
    distribute_{
        std::bind(&ServerGameRoom::receive_generic_error_notification_, this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_all_room_information_,       this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_join_room_,                  this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_leave_room_,                 this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_message_,                    this, std::placeholders::_1),
    }
{

}

ServerGameRoom::~ServerGameRoom()
{

}

const game::Room& ServerGameRoom::game_room()
{
    return *game_room_.get();
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
            io_context_,
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

void ServerGameRoom::leave_room()
{
    send_leave_room_();
    join_room_state_ = JoinRoomState::Pending;
}

void ServerGameRoom::send_message(const std::string& message)
{
    send_message_(message);
}

bool ServerGameRoom::has_message()
{
    std::shared_lock lock(message_queue_mutex_);
    return !message_queue_.empty();
}

std::tuple<std::string, time_t, std::string> ServerGameRoom::pop_message()
{
    std::unique_lock lock(message_queue_mutex_);
    auto message = message_queue_.front();
    message_queue_.pop();
    return message;
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
    net::Serializer s;
    s << room_id << nickname << is_player;
    client_->send((u_int16_t)S_Instruction::Join_Room, s.raw);
    nickname_ = nickname;
}

void ServerGameRoom::send_leave_room_()
{
    client_->send((u_int16_t)S_Instruction::Leave_Room, {});
}

void ServerGameRoom::send_message_(const std::string& message)
{
    net::Serializer s;
    s << message;
    client_->send((u_int16_t)S_Instruction::Message, s.raw);
}

void ServerGameRoom::receive_generic_error_notification_(
    const std::vector<std::byte>& data
) {

}

void ServerGameRoom::receive_all_room_information_(
    const std::vector<std::byte>& data
) {
    net::Serializer s(data);
    
    std::string name;
    u_int16_t max_users, cur_users;
    s >> name >> max_users >> cur_users;
    game_room_.reset(new game::Room(name, max_users));
    for (u_int16_t i = 0; i < cur_users; i++) {
        std::string user_name;
        s >> user_name;
        game_room_->users().push_back(
            game::User::Ptr(new game::User{.name = user_name})
        );
    }

    bool has_player;
    std::string nickname;

    s >> has_player;
    if (has_player) {
        s >> nickname;
        game::User::Ptr player = game_room_->user(nickname);
        game_room_->player(game::Chess::Color::BLACK, player);
    }

    s >> has_player;
    if (has_player) {
        s >> nickname;
        game::User::Ptr player = game_room_->user(nickname);
        game_room_->player(game::Chess::Color::WHITE, player);
    }

    s >> has_player;
    if (has_player) {
        s >> nickname;
        game::User::Ptr player = game_room_->user(nickname);
        game_room_->repentant(player);
    }

    s >> has_player;
    if (has_player) {
        s >> nickname;
        game::User::Ptr player = game_room_->user(nickname);
        game_room_->ready_user(player);
    }

    bool is_playing;
    s >> is_playing;
    game_room_->is_playing(is_playing);

    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {
            s >> game_room_->chess().board[i][j];
        }
    }

    u_int16_t record_size;
    s >> record_size;

    for (int i = 0; i < record_size; i++) {
        std::byte coor;
        s >> coor;
        game_room_->chess().record.push_back(coor);
    }

}

void ServerGameRoom::receive_join_room_(
    const std::vector<std::byte>& data
) {
    std::string nickname;
    bool is_player;
    net::Serializer s(data);
    s >> nickname >> is_player;
    if (nickname == nickname_) {
        join_room_state_ = JoinRoomState::Done;
        return;
    }
    if (is_player) {
        std::byte color;
        s >> color;
        game_room_->player_join(nickname, (game::Chess::Color)color);
    } else {
        game_room_->onlooker_join(nickname);
    }
}

void ServerGameRoom::receive_leave_room_(
    const std::vector<std::byte>& data
) {
    std::string nickname;
    net::Serializer s(data);
    s >> nickname;
    game_room_->user_leave(nickname);
}

void ServerGameRoom::receive_message_(
    const std::vector<std::byte>& data
) {
    std::string nickname, message;
    net::Serializer s(data);
    s >> nickname >> message;
    std::unique_lock lock(message_queue_mutex_);
    time_t now = time(0);
    message_queue_.emplace(nickname, now, message);
}

} // namespace gobang
} // namespace mixi