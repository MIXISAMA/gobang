#include "net/idtcp_client.h"

#include "core/io_context.h"
#include "core/log.h"
#include "net/serializer.h"

namespace mixi
{
namespace net
{

IdtcpClient::IdtcpClient(
    const boost::asio::ip::tcp::endpoint& remote_endpoint,
    const std::function<void(bool)>& connect_error_notice,
    const Distribute& distribute
) : 
    socket_(*Io_Context, boost::asio::ip::tcp::endpoint()),
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
    const std::vector<byte>& buffers
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

void IdtcpClient::handle_receive_(const boost::system::error_code& error)
{
    if (error) {
        std::ostringstream oss;
        oss << "IDTCP Client Receive Error: "
            << error.to_string();
        Log::Error(oss.str());
        return;
    }
    
    std::ostringstream oss;
    oss << "IDTCP Client Received ["
        << receive_instruction_
        << "]";
    Log::Info(oss.str());

    if (receive_instruction_ >= distribute_.size()) {
        Log::Error("Instruction out of distribute size");
    } else {
        distribute_[receive_instruction_](receive_data_);
    }

    start_receive_();
}

void IdtcpClient::handle_send_(const boost::system::error_code& error)
{
    if (error) {
        std::ostringstream oss;
        oss << "IDTCP Client Send Error: "
            << error.to_string();
        Log::Error(oss.str());
    }
    Log::Info("IDTCP Client Send Success");
}

void IdtcpClient::start_receive_()
{
    socket_.async_receive_instrution_data(
        receive_instruction_, receive_data_,
        boost::bind(
            &IdtcpClient::handle_receive_, this,
            boost::asio::placeholders::error
        )
    );
}

} // namespace net
} // namespace mixi