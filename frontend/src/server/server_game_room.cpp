#include "server/server_game_room.h"

#include "core/log.h"
#include "net/serializer.h"

namespace mixi
{
namespace gobang
{

ServerGameRoom::ServerGameRoom(
    boost::asio::io_context& io_context
) :
    io_context(io_context),
    join_room_state_(JoinRoomState::Pending),
    distribute_{
        std::bind(&ServerGameRoom::receive_fatal_error_, this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_public_key_,  this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_join_room_,   this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_leave_room_,  this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_message_,     this, std::placeholders::_1),
    },
    client_(io_context, distribute_)
{

}

ServerGameRoom::~ServerGameRoom()
{

}

void ServerGameRoom::join_room(
    const boost::asio::ip::tcp::endpoint& remote_endpoint,
    uint8_t room_id,
    const std::string& username,
    const std::string& password,
    char role
) {

    room_id_ = room_id;
    username_ = username;
    password_ = password;
    role_ = role;

    boost::asio::co_spawn(
        io_context,
        client_.connect(remote_endpoint),
        boost::asio::detached
    );
}

ServerGameRoom::JoinRoomState ServerGameRoom::join_room_state()
{
    return join_room_state_;
}

void ServerGameRoom::leave_room()
{
    boost::asio::co_spawn(
        io_context,
        send_leave_room_(),
        boost::asio::detached
    );
}

void ServerGameRoom::send_message(const std::string& message)
{
    boost::asio::co_spawn(
        io_context,
        send_send_message_(message),
        boost::asio::detached
    );
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

boost::asio::awaitable<void> ServerGameRoom::send_join_room_()
{
    
}

boost::asio::awaitable<void> ServerGameRoom::send_leave_room_()
{
    co_await client_.send((u_int16_t)S_Instruction::Leave_Room, {});
}

boost::asio::awaitable<void> ServerGameRoom::send_send_message_(const std::string& message)
{
    net::Serializer s;
    s << message;
    co_await client_.send((u_int16_t)S_Instruction::Message, s.raw);
}

void ServerGameRoom::receive_fatal_error_(
    const std::vector<std::byte>& data
) {
    net::Serializer s(data);
    int32_t error_id;
    std::string detail;
    s.head8();
    s >> error_id >> detail;
    Log::Error(std::to_string(error_id) + " " + detail);
}


void ServerGameRoom::receive_public_key_(
    const std::vector<std::byte>& data
) {
    net::Serializer s(data);
    std::vector<std::byte> pubkey;
    std::vector<std::byte> uuid;
    s.head8();
    s >> pubkey >> uuid;

    boost::crypto3::rsa::public_key<boost::crypto3::rsa::private_key<>> pubKey;
    pubKey.load(boost::crypto3::rsa::make_public_key_from_der(publicKey));

    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    client_uuid_.reserve(16);
    std::memcpy(client_uuid_.data(), &uuid, sizeof(uuid));
    co_await send_join_room_()
}

void ServerGameRoom::receive_you_join_room_(
    const std::vector<std::byte>&data
) {
    net::Serializer s(data);
    bool is_success;
    s >> is_success;
    if (!is_success) {
        return;
    }
    ReadFirstBuffer<game::Room>::W room = game_room_.w();
    s >> room->name
      >> room->max_users
      >> room->black_player
      >> room->white_player;
    uint8_t onlookers_size;
    s >> onlookers_size;
    room->onlookers.resize(onlookers_size);
    for (uint8_t i = 0; i < onlookers_size; i++)
    {
        s >> room->onlookers[i];
    }
    s >> room->ready_player
      >> room->is_playering
      >> room->regret_player
      >> room->tie_player
      >> room->records;

}

void ServerGameRoom::receive_user_info_(
    const std::vector<std::byte>& data
) {
    // net::Serializer s(data);
    
    // std::string name;
    // u_int16_t max_users, cur_users;
    // s >> name >> max_users >> cur_users;
    // // game_room_.reset(new game::Room(name, max_users));
    // for (u_int16_t i = 0; i < cur_users; i++) {
    //     std::string user_name;
    //     s >> user_name;
    //     game_room_.onlooker_join(user_name);
    //     // game_room_->users().push_back(
    //     //     game::User::Ptr(new game::User{.name = user_name})
    //     // );
    // }

    // bool has_player;
    // std::string nickname;

    // s >> has_player;
    // if (has_player) {
    //     s >> nickname;
    //     game::User::Ptr player = game_room_->user(nickname);
    //     game_room_->player(game::Chess::Color::BLACK, player);
    // }

    // s >> has_player;
    // if (has_player) {
    //     s >> nickname;
    //     game::User::Ptr player = game_room_->user(nickname);
    //     game_room_->player(game::Chess::Color::WHITE, player);
    // }

    // s >> has_player;
    // if (has_player) {
    //     s >> nickname;
    //     game::User::Ptr player = game_room_->user(nickname);
    //     game_room_->repentant(player);
    // }

    // s >> has_player;
    // if (has_player) {
    //     s >> nickname;
    //     game::User::Ptr player = game_room_->user(nickname);
    //     game_room_->ready_user(player);
    // }

    // bool is_playing;
    // s >> is_playing;
    // game_room_->is_playing(is_playing);

    // for (int i = 0; i < 15; i++) {
    //     for (int j = 0; j < 15; j++) {
    //         s >> game_room_->chess().board[i][j];
    //     }
    // }

    // u_int16_t record_size;
    // s >> record_size;

    // for (int i = 0; i < record_size; i++) {
    //     std::byte coor;
    //     s >> coor;
    //     game_room_->chess().record.push_back(coor);
    // }

}

void ServerGameRoom::receive_join_room_(
    const std::vector<std::byte>& data
) {
    // std::string nickname;
    // bool is_player;
    // net::Serializer s(data);
    // s >> nickname >> is_player;
    // if (nickname == nickname_) {
    //     join_room_state_ = JoinRoomState::Done;
    //     return;
    // }
    // if (is_player) {
    //     std::byte color;
    //     s >> color;
    //     game_room_->player_join(nickname, (game::Chess::Color)color);
    // } else {
    //     game_room_->onlooker_join(nickname);
    // }
}

void ServerGameRoom::receive_leave_room_(
    const std::vector<std::byte>& data
) {
    // std::string nickname;
    // net::Serializer s(data);
    // s >> nickname;
    // game_room_->user_leave(nickname);
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