#include "net/server_game_room.h"

#include "core/io_context.h"
#include "net/serializer.h"
#include "core/log.h"

namespace mixi
{
namespace gobang
{

ServerGameRoom::ServerGameRoom(
    u_int16_t room_id,
    const std::string& nickname,
    bool is_player,
    const boost::asio::ip::tcp::endpoint& remote_endpoint
) :
    room_(nullptr),
    room_id_(room_id),
    is_player_(is_player),
    state_(State::Loading),
    nickname_(nickname),
    remote_endpoint_(remote_endpoint),
    socket_(*Io_Context, boost::asio::ip::tcp::endpoint())
{
    socket_.async_connect(remote_endpoint_, 
        boost::bind(&ServerGameRoom::handle_connect_, this,
            boost::asio::placeholders::error
        )
    );
}

ServerGameRoom::~ServerGameRoom()
{
    socket_.close();
}

const GameRoom* ServerGameRoom::room() const
{
    return room_;
}



ServerGameRoom::State ServerGameRoom::state()
{
    return state_;
}

void ServerGameRoom::request_room()
{
    // socket_.async_send(remote_endpoint_,
    //     boost::bind(&ServerGameRoom::handle_send_, this)
    // );
}

void ServerGameRoom::start_receive_()
{
    socket_.async_receive_instrution_data(
        receive_instruction_, receive_data_,
        boost::bind(&ServerGameRoom::handle_receive_, this,
            boost::asio::placeholders::error
        )
    );
}

void ServerGameRoom::handle_connect_(const boost::system::error_code& error)
{
    if (error) {
        state_ = State::Fail;
        return;
    }
    start_receive_();
    join_();
}

void ServerGameRoom::handle_send_(const boost::system::error_code& error)
{

}

void ServerGameRoom::handle_receive_(const boost::system::error_code& error)
{
    if (error) {
        Log::Error(error.to_string());
        return;
    }
    std::cout << "handle receive: " << receive_instruction_ << std::endl;
    start_receive_();
}

void ServerGameRoom::handle_join_(const boost::system::error_code& error, std::size_t)
{
    if (error) {
        Log::Error("join failed");
        Log::Error(error.to_string());
        state_ = State::Fail;
        return;
    }
}

void ServerGameRoom::join_()
{
    Serializer s;
    s << room_id_ << nickname_;
    u_int16_t instruction = (u_int16_t)(is_player_ ?
        Instruction::Join_As_Player :
        Instruction::Join_As_Onlooker
    );
    socket_.async_send_instrution_data(
        instruction, s.raw,
        boost::bind(&ServerGameRoom::handle_join_, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred
        )
    );
}

} // namespace gobang
} // namespace mixi