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
    client_(io_context, {
        std::bind(&ServerGameRoom::receive_fatal_error_,     this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_public_key_,      this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_you_join_room_,   this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_user_info_,       this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_other_join_room_, this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_leave_room_,      this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_player_ready_,    this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_game_start_,      this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_player_stone_,    this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_player_regret_,   this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_agree_regret_,    this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_player_tie_,      this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_agree_tie_,       this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_game_over_,       this, std::placeholders::_1),
        std::bind(&ServerGameRoom::receive_message_,         this, std::placeholders::_1),
    },  std::bind(&ServerGameRoom::on_disconnected_,         this))
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
    signal_join_room_(JoinRoomState::JOINING);

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

boost::signals2::connection ServerGameRoom::connect_join_room(
    const std::function<void(JoinRoomState)>& callback
) {
    return signal_join_room_.connect(callback);
}

boost::signals2::connection ServerGameRoom::connect_agree_regret(
    const std::function<void(bool)>& callback
) {
    return signal_agree_regret_.connect(callback);
}
boost::signals2::connection ServerGameRoom::connect_agree_tie(
    const std::function<void(bool)>& callback
) {
    return signal_agree_tie_.connect(callback);
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

void ServerGameRoom::on_disconnected_()
{
    Log::Info("disconnected");
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
    const unsigned char* pubkeybytes = (const unsigned char*)pubkey.data();
    EVP_PKEY* pkey = d2i_PUBKEY(NULL, &pubkeybytes, pubkey.size());
    assert(pkey);
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    assert(ctx);
    res = EVP_PKEY_encrypt_init(ctx);
    assert(res > 0);
    res = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING);
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

    std::vector<std::byte> client_uuid_(16);
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    std::memcpy(client_uuid_.data(), uuid.data, 16);

    net::Serializer s3;
    s3.head8();
    s3 << username_ << cipher_bytes << client_uuid_;
    
    boost::asio::co_spawn(
        io_context,
        client_.send((u_int16_t)S_Instruction::Join_Room, std::vector<std::byte>(s3.raw)),
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
        signal_join_room_(JoinRoomState::FAILED);
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
    room->update_board_by_records();

    signal_join_room_(JoinRoomState::JOINED);
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

void ServerGameRoom::receive_player_ready_(
    const std::vector<std::byte>& data
) {
    net::Serializer s(data);
    RfbWriter<game::Room> room(game_room_);
    s >> room->ready_player;
}

void ServerGameRoom::receive_game_start_(
    const std::vector<std::byte>& data
) {
    std::string black_player;
    net::Serializer s(data);
    s >> black_player;
    RfbWriter<game::Room> room(game_room_);
    if (room->black_player != black_player) {
        std::swap(room->black_player, room->white_player);
    }
    room->is_playering = true;
}

void ServerGameRoom::receive_player_stone_(
    const std::vector<std::byte>& data
) {
    std::byte coor;
    net::Serializer s(data);
    s >> coor;
    RfbWriter<game::Room> room(game_room_);
    room->stone(coor);
}

void ServerGameRoom::receive_player_regret_(
    const std::vector<std::byte>& data
) {
    net::Serializer s(data);
    RfbWriter<game::Room> room(game_room_);
    s >> room->regret_player;
}

void ServerGameRoom::receive_agree_regret_(
    const std::vector<std::byte>& data
) {
    bool agree;
    net::Serializer s(data);
    s >> agree;
    signal_agree_regret_(agree);
}

void ServerGameRoom::receive_player_tie_(
    const std::vector<std::byte>& data
) {
    net::Serializer s(data);
    RfbWriter<game::Room> room(game_room_);
    s >> room->tie_player;
}

void ServerGameRoom::receive_agree_tie_(
    const std::vector<std::byte>& data
) {
    bool agree;
    net::Serializer s(data);
    s >> agree;
    signal_agree_tie_(agree);
}

void ServerGameRoom::receive_game_over_(
    const std::vector<std::byte>& data
) {
    std::byte winner;
    bool surrender;
    net::Serializer s(data);
    s >> winner >> surrender;
    // todo
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