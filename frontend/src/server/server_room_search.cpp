#include "server/server_room_search.h"

#include "core/log.h"

#include "net/serializer.h"

namespace mixi
{
namespace gobang
{

ServerRoomSearch::ServerRoomSearch(boost::asio::io_context& io_context) :
    io_context_(io_context),
    socket_(io_context, boost::asio::ip::udp::endpoint())
{
    socket_.set_option(boost::asio::ip::udp::socket::broadcast(true));
    boost::asio::co_spawn(io_context_, receive_(), boost::asio::detached);
}

ServerRoomSearch::~ServerRoomSearch()
{
    socket_.close();
}

void ServerRoomSearch::search_room(const boost::asio::ip::udp::endpoint &endpoint)
{
    boost::asio::co_spawn(io_context_, send_search_room_(endpoint), boost::asio::detached);
}

ReadTryQueue<ConciseRoom>& ServerRoomSearch::rooms()
{
    return rooms_;
}

boost::asio::awaitable<void> ServerRoomSearch::receive_()
{
    std::vector<std::byte> recv_buffer_(1 << 16);
    boost::asio::ip::udp::endpoint sender_endpoint_;
    while (true) {     
        co_await socket_.async_receive_from(
            boost::asio::buffer(recv_buffer_),
            sender_endpoint_,
            boost::asio::use_awaitable
        );
        Log::Info("ServerRoomSearch received");

        net::Serializer s(recv_buffer_);
        s.head8();
        std::string version;
        std::string server_name;
        s >> version >> server_name;
        uint8_t room_num;
        s >> room_num;
        for (uint8_t i = 0; i < room_num; i++) {
            ConciseRoom room;
            room.room_id = i;

            s >> room.room_name
              >> room.is_playing
              >> room.black_player
              >> room.white_player
              >> room.users
              >> room.max_users;

            room.endpoint.address(sender_endpoint_.address());
            room.endpoint.port(sender_endpoint_.port());

            RtqWriter<ConciseRoom> rooms(rooms_);
            rooms.push(room);
        }
    }
}

boost::asio::awaitable<void> ServerRoomSearch::send_search_room_(const boost::asio::ip::udp::endpoint endpoint)
{
    Log::Info("ServerRoomSearch send search room");
    net::Serializer s;
    s << VERSION;
    co_await socket_.async_send_to(
        boost::asio::buffer(s.raw),
        endpoint,
        boost::asio::use_awaitable
    );
}

bool ConciseRoom::operator < (const ConciseRoom& other) const
{
    if (this->version == other.version) {
        return this->room_id < other.room_id;
    }
    return this->version < other.version;
}

} // namespace gobang
} // namespace mixi