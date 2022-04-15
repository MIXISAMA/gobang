#include "net/server_room_search.h"

#include "core/io_context.h"
#include "core/log.h"

namespace mixi
{
namespace gobang
{

ServerRoomSearch::ServerRoomSearch() :
    socket_(*Io_Context, boost::asio::ip::udp::endpoint())
{
    socket_.set_option(boost::asio::ip::udp::socket::broadcast(true));
}

ServerRoomSearch::~ServerRoomSearch()
{
    socket_.close();
}


void ServerRoomSearch::start_receive_()
{
    std::cout << "start_receive_" << std::endl;
    std::cout 
        << "local_endpoint "
        << socket_.local_endpoint().address().to_string()
        << ":" << socket_.local_endpoint().port() << std::endl;

    socket_.async_receive(
        boost::asio::buffer(recv_buffer_),
        // sender_endpoint_,
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
        // return;
    }
    std::cout
        << "sender endpoint " 
        << sender_endpoint_.address().to_string()
        << ":" << sender_endpoint_.port() << std::endl;

    std::cout << recv_buffer_.size() << std::endl;
    start_receive_();
}

void ServerRoomSearch::boardcast_search_room()// const
{
    std::cout
        << "sender endpoint " 
        << sender_endpoint_.address().to_string()
        << ":" << sender_endpoint_.port() << std::endl;

    boost::asio::ip::udp::endpoint endpoint(
        boost::asio::ip::address_v4::broadcast(),
        52039 // Todo: remove hard code
    );

    socket_.async_send_to(
        boost::asio::buffer(VERSION),
        endpoint,
        boost::bind(&ServerRoomSearch::handle_send_, this,
            boost::asio::placeholders::error
        )
    );
}

void ServerRoomSearch::handle_send_(
    const boost::system::error_code& error
    // std::size_t /*bytes_transferred*/
) {
    start_receive_();
}

} // namespace gobang
} // namespace mixi