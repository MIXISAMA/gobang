#include "net/server_room_search.h"

#include "core/io_context.h"
#include "core/log.h"

#include "net/serializer.h"

namespace mixi
{
namespace gobang
{

ServerRoomSearch::ServerRoomSearch() :
    recv_buffer_(1 << 16),
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
        // return;
    }
    std::cout
        << "sender endpoint " 
        << sender_endpoint_.address().to_string()
        << ":" << sender_endpoint_.port() << std::endl;

    Serializer s(recv_buffer_);

    int room_num = s.read_uint16(); 
    std::cout << "room number: " << room_num << std::endl;

    for (int i = 0; i < room_num; i++) {
        int room_id = s.read_uint16();
        std::string room_name = s.read_string();
        std::cout << room_id << ": " << room_name << std::endl;
        for (int j = 0; j < 2; j++) {
            std::cout << "p" << j + 1 << std::endl;
            if (s.read_boolean()) {
                std::string player_name = s.read_string();
                std::cout << player_name << std::endl;
            }
        }
        int onlooker_num = s.read_uint16();
        std::cout << "onlooker number: " << onlooker_num << std::endl;
    }

    start_receive_();
}

void ServerRoomSearch::boardcast_search_room()// const
{
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