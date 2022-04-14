#include "net/udp_server.h"

#include "core/log.h"

namespace mixi
{
namespace gobang
{

UdpServer* UdpServer::instance_ = nullptr;

UdpServer::UdpServer(boost::asio::io_context& io_context) :
    socket_(io_context)
{
    socket_.open(boost::asio::ip::udp::v4());
    socket_.set_option(boost::asio::ip::udp::socket::broadcast(true));

    boardcast_search_room();
}

UdpServer::~UdpServer()
{
    socket_.close();
}

void UdpServer::Init(boost::asio::io_context& io_context)
{
    instance_ = new UdpServer(io_context);
}

UdpServer* UdpServer::Instance()
{
    return instance_;
}

void UdpServer::start_receive_()
{
    std::cout << "start_receive_" << std::endl;
    std::cout 
        << "local_endpoint "
        << socket_.local_endpoint().address().to_string()
        << ":" << socket_.local_endpoint().port() << std::endl;

    socket_.async_receive(
        boost::asio::buffer(recv_buffer_),
        // sender_endpoint_,
        boost::bind(&UdpServer::handle_receive_, this,
            boost::asio::placeholders::error
            // boost::asio::placeholders::bytes_transferred
        )
    );
}

void UdpServer::handle_receive_(
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

void UdpServer::boardcast_search_room()// const
{
    std::cout
        << "sender endpoint " 
        << sender_endpoint_.address().to_string()
        << ":" << sender_endpoint_.port() << std::endl;

    boost::asio::ip::udp::endpoint endpoint(
        boost::asio::ip::address_v4::broadcast(),
        52039 // Todo: remove hard code
    );
    std::vector<byte> send_buf{ 0 };
    socket_.async_send_to(
        boost::asio::buffer(send_buf),
        endpoint,
        boost::bind(&UdpServer::handle_send_, this,
            boost::asio::placeholders::error
        )
    );
}

void UdpServer::handle_send_(
    const boost::system::error_code& error
    // std::size_t /*bytes_transferred*/
) {
    start_receive_();
}

} // namespace gobang
} // namespace mixi