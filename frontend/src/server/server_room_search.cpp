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

boost::asio::awaitable<void> ServerRoomSearch::receive_()
{
    Log::Info("ServerRoomSearch receive_");
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
        u_int16_t room_num;// = s.read_uint16();
        s >> room_num;
        for (u_int16_t i = 0; i < room_num; i++) {
            ConciseRoom room;
            s >> room.id >> room.name >> room.is_playing;
            for (int j = 0; j < 2; j++) {
                bool exist_player;
                s >> exist_player;
                if (exist_player) {
                    s >> room.player_name[j];
                }
            }
            s >> room.onlooker_num >> room.max_onlooker_num;
            room.endpoint.address(sender_endpoint_.address());
            room.endpoint.port(sender_endpoint_.port());

            std::unique_lock lock(rooms_mutex_);
            rooms_.push_back(room);
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

std::vector<ConciseRoom> ServerRoomSearch::new_rooms()
{
    if (rooms_mutex_.try_lock()) {
        std::vector<ConciseRoom> new_rooms = std::move(rooms_);
        rooms_mutex_.unlock();
        return new_rooms;
    }
    return {};
}

bool ConciseRoom::operator < (const ConciseRoom& room) const
{
    if (this->id == room.id) {
        return this->endpoint < room.endpoint;
    }
    return this->id < room.id;
}

} // namespace gobang
} // namespace mixi