#pragma once
#include "pch.h"

namespace mixi
{
namespace net
{

// std::vector<byte> IdtcpPack(
//     u_int16_t instruction,
//     const std::vector<byte>& data
// );

// std::pair<uint16_t, std::vector<byte>> IdtcpUnpack(
//     const std::vector<byte>& idtcp_raw
// );


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
        const std::vector<byte>& buffers,
        const boost::function<void(const boost::system::error_code&, std::size_t)>& handler
    );

    void async_receive_instrution_data(
        u_int16_t& instruction,
        std::vector<std::byte>& data,
        const boost::function<void(const boost::system::error_code&)>& handler
    );

protected:

    std::byte send_buffer_[65536];
    std::byte buffer_[65536];

    void pack_(
        u_int16_t instruction,
        const std::vector<byte>& data,
        std::byte* idtcp_raw,
        u_int16_t& idtcp_raw_bytes
    );

    void unpack_(
        const byte* idtcp_raw,
        u_int16_t data_bytes,
        u_int16_t& instruction,
        std::vector<byte>& data
    );

    void handle_receive_(
        const boost::system::error_code& error,
        std::size_t bytes_transferred,
        u_int16_t& instruction,
        std::vector<std::byte>& data,
        const boost::function<void(const boost::system::error_code&)>& handler
    );


};

} // namespace net
} // namespace mixi
