#pragma once
#include "pch.h"

#include "net/idtcp.h"

namespace mixi
{
namespace net
{

class IdtcpClient
{

public:

    using Distribute = std::vector<std::function<void(const std::vector<std::byte>&)>>;

    IdtcpClient(
        const boost::asio::ip::tcp::endpoint& remote_endpoint,
        const std::function<void(bool)>& connect_error_notice,
        const Distribute& distribute
    );
    ~IdtcpClient();

    void send(u_int16_t instruction, const std::vector<byte>& buffers);

protected:

    net::IdtcpSocket socket_;

    std::function<void(bool)> connect_error_notice_;
    Distribute distribute_;

    void start_receive_();
    
    void handle_connect_(const boost::system::error_code& error);
    void handle_receive_(u_int16_t instruction, const std::vector<std::byte>& data);
    void handle_send_   (const boost::system::error_code& error);

private:

};

} // namespace net
} // namespace mixi
