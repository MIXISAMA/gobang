#pragma once
#include "pch.h"

namespace mixi
{
namespace net
{

class IdtcpSocket : public boost::asio::ip::tcp::socket
{

public:

    IdtcpSocket(
        boost::asio::io_context& io_context,
        const boost::asio::ip::tcp::endpoint local_endpoint
    );
    ~IdtcpSocket() = default;

    void async_send_instrution_data(
        u_int16_t instruction,
        const std::vector<std::byte>& buffers,
        const boost::function<void(const boost::system::error_code&, std::size_t)>& handler
    );

    void async_receive_instrution_data(
        const std::function<
            void(u_int16_t, const std::vector<std::byte>&)
        >& handler
    );

protected:

    std::byte send_buffer_[65536];
    std::byte buffer_[65536];

    void pack_(
        u_int16_t instruction,
        const std::vector<std::byte>& data,
        std::byte* idtcp_raw,
        u_int16_t& idtcp_raw_bytes
    );

    void unpack_(
        const std::byte* idtcp_raw,
        u_int16_t data_bytes,
        u_int16_t& instruction,
        std::vector<std::byte>& data
    );

    void handle_receive_(
        const boost::system::error_code& error,
        std::size_t bytes_transferred,
        const std::function<
            void(u_int16_t, const std::vector<std::byte>&)
        >& handler
    );


};

} // namespace net
} // namespace mixi
