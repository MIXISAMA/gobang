#include "server/server_game_room.h"

#include "core/log.h"
#include "net/serializer.h"
#include "game/chess.h"

namespace mixi
{
namespace gobang
{

ServerGameRoom::ServerGameRoom(
    boost::asio::io_context& io_context
) :
    io_context(io_context),
    distribute_{
        std::bind(&ServerGameRoom::receive_fatal_error_, this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_public_key_,  this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_other_join_room_, this, std::placeholders::_1),
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
    join_room_signal(JoinRoomState::JOINING);

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

void ServerGameRoom::on_join_room(
    const std::function<void(JoinRoomState)>& callback
) {
    join_room_signal.connect(callback);
}

const ReadFirstBuffer<game::Room>& ServerGameRoom::room() const
{
    return game_room_;
}

ReadTryQueue<game::User>& ServerGameRoom::users()
{
    return users_;
}

ReadTryQueue<msg_t>& ServerGameRoom::messages()
{
    return messages_;
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
    net::Serializer s1(data);
    std::vector<std::byte> pubkey;
    std::vector<std::byte> server_uuid;
    s1.head8();
    s1 >> pubkey >> server_uuid;

    net::Serializer s2;
    s2.head8();
    s2 << password_ << server_uuid;


    int res = 0;
    BIO* bio = BIO_new_mem_buf(pubkey.data(), pubkey.size());
    EVP_PKEY* pkey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    assert(pkey);
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    assert(ctx);
    res = EVP_PKEY_encrypt_init(ctx);
    assert(res > 0);
    res = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING);
    assert(res > 0);
    unsigned char* plain = (unsigned char*)s2.raw.data();
    size_t plain_size = s2.raw.size();
    size_t cipher_size;
    res = EVP_PKEY_encrypt(ctx, nullptr, &cipher_size, plain, plain_size);
    assert(res > 0);
    std::vector<std::byte> cipher_bytes(cipher_size);
    unsigned char* cipher = (unsigned char*)cipher_bytes.data();
    res = EVP_PKEY_encrypt(ctx, cipher, &cipher_size, plain, plain_size);
    assert(res > 0);
    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    BIO_free(bio);

    std::vector<std::byte> client_uuid_(16);
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    std::memcpy(client_uuid_.data(), uuid.data, 16);

    net::Serializer s3;
    s3.head8();
    s3 << username_ << cipher_bytes << client_uuid_;
    
    boost::asio::co_spawn(
        io_context,
        client_.send((u_int16_t)S_Instruction::Join_Room, s3.raw),
        boost::asio::detached
    );
}

void ServerGameRoom::receive_you_join_room_(
    const std::vector<std::byte>&data
) {
    net::Serializer s(data);
    bool is_success;
    s >> is_success;
    if (!is_success) {
        join_room_signal(JoinRoomState::FAILED);
        return;
    }

    RfbWriter<game::Room> room(game_room_);
    s >> room->name
      >> room->max_users
      >> room->black_player
      >> room->white_player;
    uint8_t onlookers_size;
    s >> onlookers_size;
    room->onlookers.resize(onlookers_size);
    for (uint8_t i = 0; i < onlookers_size; i++) {
        s >> room->onlookers[i];
    }
    s >> room->ready_player
      >> room->is_playering
      >> room->regret_player
      >> room->tie_player
      >> room->records;

    join_room_signal(JoinRoomState::JOINED);
}

void ServerGameRoom::receive_user_info_(
    const std::vector<std::byte>& data
) {
    net::Serializer s(data);
    game::User user;
    s >> user.username
      >> user.number_of_wins
      >> user.number_of_ties
      >> user.number_of_losses
      >> user.number_of_matches
      >> user.game_duration;
    RtqWriter<game::User> users(users_);
    users.push(user);
}

void ServerGameRoom::receive_other_join_room_(
    const std::vector<std::byte>& data
) {
    std::string username;
    std::byte role;
    net::Serializer s(data);
    s >> username >> role;
    RfbWriter<game::Room> room(game_room_);
    room->user_join(username, role);
}

void ServerGameRoom::receive_leave_room_(
    const std::vector<std::byte>& data
) {
    std::string username;
    net::Serializer s(data);
    s >> username;
    RfbWriter<game::Room> room(game_room_);
    room->user_leave(username);
}

void ServerGameRoom::receive_message_(
    const std::vector<std::byte>& data
) {
    std::string username;
    uint64_t datetime;
    std::string message;
    net::Serializer s(data);
    s.head8();
    s >> username >> datetime;
    s.head16();
    s >> message;
    RtqWriter<msg_t> messages(messages_);
    messages.emplace(username, datetime, message);
}

} // namespace gobang
} // namespace mixi