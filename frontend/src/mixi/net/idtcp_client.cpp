#include "net/idtcp_client.h"

#include "core/log.h"
#include "net/serializer.h"

namespace mixi
{
namespace net
{

IdtcpClient::IdtcpClient(
    boost::asio::io_context& io_context,
    const boost::asio::ip::tcp::endpoint& remote_endpoint,
    const std::function<void(bool)>& connect_error_notice,
    const Distribute& distribute
) : 
    socket_(io_context, boost::asio::ip::tcp::endpoint()),
    connect_error_notice_(connect_error_notice),
    distribute_(distribute)
{
    socket_.async_connect(
        remote_endpoint,
        boost::bind(
            &IdtcpClient::handle_connect_, this,
            boost::asio::placeholders::error
        )
    );
}

IdtcpClient::~IdtcpClient()
{
    socket_.close();
}

void IdtcpClient::send(
    u_int16_t instruction,
    const std::vector<std::byte>& buffers
) {
    socket_.async_send_instrution_data(
        instruction, buffers,
        boost::bind(
            &IdtcpClient::handle_send_, this,
            boost::asio::placeholders::error
        )
    );
}

void IdtcpClient::handle_connect_(const boost::system::error_code& error)
{
    connect_error_notice_((bool)error);
    if (error) {
        return;
    }
    start_receive_();
}

void IdtcpClient::handle_receive_(
    u_int16_t instruction,
    const std::vector<std::byte>& data
) {

    std::ostringstream oss;
    oss << "IDTCP Client Received [" << instruction << "]";
    Log::Info(oss.str());

    if (instruction >= distribute_.size()) {
        Log::Error("Instruction out of distribute size");
    } else {
        distribute_[instruction](data);
    }

    start_receive_();
}

void IdtcpClient::handle_send_(const boost::system::error_code& error)
{
    if (error) {
        std::ostringstream oss;
        oss << "IDTCP Client Send Error: "
            << error.message();
        Log::Error(oss.str());
    }
    Log::Info("IDTCP Client Send Success");
}

void IdtcpClient::start_receive_()
{
    socket_.async_receive_instrution_data(
        std::bind(
            &IdtcpClient::handle_receive_, this,
            std::placeholders::_1,
            std::placeholders::_2
        )
    );
}

} // namespace net
} // namespace mixi