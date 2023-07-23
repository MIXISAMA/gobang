#pragma once
#include "mixi/pch.h"

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
        uint16_t instruction,
        const std::vector<std::byte>& buffers,
        const boost::function<void(const boost::system::error_code&, std::size_t)>& handler
    );

    void async_receive_instrution_data(
        const std::function<
            void(uint16_t, const std::vector<std::byte>&)
        >& handler
    );

    boost::asio::awaitable<void>
    send_instrution_data(
        uint16_t instruction,
        const std::vector<std::byte>& buffers
    );

    boost::asio::awaitable<std::pair<uint16_t, std::vector<std::byte>>>
    receive_instrution_data();

protected:

    std::byte send_buffer_[65536];
    std::byte buffer_[65536];

    void pack_(
        uint16_t instruction,
        const std::vector<std::byte>& data,
        std::byte* idtcp_raw,
        uint16_t& idtcp_raw_bytes
    );

    void unpack_(
        const std::byte* idtcp_raw,
        uint16_t data_bytes,
        uint16_t& instruction,
        std::vector<std::byte>& data
    );

    void handle_receive_(
        const boost::system::error_code& error,
        std::size_t bytes_transferred,
        const std::function<
            void(uint16_t, const std::vector<std::byte>&)
        >& handler
    );


};

class IdtcpClient
{

public:

    using Distribute = std::function<void(const std::vector<std::byte>&)>;

    IdtcpClient(
        boost::asio::io_context& io_context,
        std::initializer_list<Distribute> distribution,
        const std::function<void(void)>& disconnected_callback
    );
    ~IdtcpClient();

    boost::asio::awaitable<void>
    connect(const boost::asio::ip::tcp::endpoint& remote_endpoint);

    boost::asio::awaitable<void>
    send(
        uint16_t instruction,
        const std::vector<std::byte> buffers
    );

    bool connected() const;

protected:

    net::IdtcpSocket socket_;
    std::vector<Distribute> distribution_;
    std::function<void(void)> on_disconnected_;
    std::atomic<bool> connected_;

    boost::asio::awaitable<void> start_receive_();

private:

};

} // namespace net
} // namespace mixi
