#include "server/server_room_search.h"

#include "core/io_context.h"
#include "core/log.h"

#include "net/serializer.h"

namespace mixi
{
namespace gobang
{

ServerRoomSearch::ServerRoomSearch() :
    recv_buffer_(1 << 16),
    socket_(*Io_Context, boost::asio::ip::udp::endpoint()),
    has_new_rooms_(false)
{
    socket_.set_option(boost::asio::ip::udp::socket::broadcast(true));
    start_receive_();
}

ServerRoomSearch::~ServerRoomSearch()
{
    socket_.close();
}

void ServerRoomSearch::update_rooms()
{
    if (has_new_rooms_) {
        std::shared_lock lock(room_set_mutex_);
        rooms_.assign(room_set_.begin(), room_set_.end());
        has_new_rooms_ = false;
    }
}

void ServerRoomSearch::search_room(const boost::asio::ip::udp::endpoint &endpoint)
{
    room_set_.clear();
    net::Serializer s;
    s << VERSION;
    socket_.async_send_to(
        boost::asio::buffer(s.raw),
        endpoint,
        boost::bind(&ServerRoomSearch::handle_send_, this,
            boost::asio::placeholders::error//,
            // boost::asio::placeholders::bytes_transferred
        )
    );
}

const std::vector<ConciseRoom>& ServerRoomSearch::rooms()
{
    return rooms_;
}

void ServerRoomSearch::start_receive_()
{
    socket_.async_receive_from(
        boost::asio::buffer(recv_buffer_),
        sender_endpoint_,
        boost::bind(&ServerRoomSearch::handle_receive_, this,
            boost::asio::placeholders::error
            // boost::asio::placeholders::bytes_transferred
        )
    );
}

void ServerRoomSearch::handle_receive_(
    const boost::system::error_code& error
    // std::size_t /*bytes_transferred*/
) {
    if (error) {
        Log::Error(error.to_string());
        return;
    }

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

        std::unique_lock lock(room_set_mutex_);
        auto [_, success] = room_set_.insert(room);
        if (success) {
            has_new_rooms_ = true;
        }
    }

    start_receive_();
}

void ServerRoomSearch::handle_send_(
    const boost::system::error_code& error
    // std::size_t /*bytes_transferred*/
) {
    // start_receive_();
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