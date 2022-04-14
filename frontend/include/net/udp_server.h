#pragma once

#include "pch.h"
#include "game/room.h"

namespace mixi
{
namespace gobang
{

class UdpServer
{

public:

    ~UdpServer();

    static void Init(boost::asio::io_context& io_context);

    static UdpServer* Instance();

    void boardcast_search_room();

private:

    UdpServer(boost::asio::io_context& io_context);

    static UdpServer* instance_;

    void start_receive_();
    void handle_receive_(
        const boost::system::error_code& error
        // std::size_t /*bytes_transferred*/
    );
    void handle_send_(const boost::system::error_code& error);

    boost::asio::ip::udp::socket socket_;
    boost::array<char, 128>  recv_buffer_;
    boost::asio::ip::udp::endpoint sender_endpoint_;

    std::set<Room> rooms_;
    std::shared_mutex rooms_mutex_;

};

} // namespace gobang
} // namespace mixi
