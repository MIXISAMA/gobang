#pragma once

#include "pch.h"
#include "game/room.h"

namespace mixi
{
namespace gobang
{

class ServerRoomSearch
{

public:

    ServerRoomSearch();
    ~ServerRoomSearch();

    void boardcast_search_room();

private:

    void start_receive_();
    void handle_receive_(
        const boost::system::error_code& error
        // std::size_t /*bytes_transferred*/
    );
    void handle_send_(const boost::system::error_code& error);

    boost::asio::ip::udp::socket socket_;
    std::vector<std::byte>  recv_buffer_;
    boost::asio::ip::udp::endpoint sender_endpoint_;

    std::set<Room> rooms_;
    std::shared_mutex rooms_mutex_;

};

} // namespace gobang
} // namespace mixi
