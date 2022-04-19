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

    template <
        BOOST_ASIO_COMPLETION_TOKEN_FOR(
            void (boost::system::error_code, std::size_t)
        ) WriteHandler BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)
    >
    BOOST_ASIO_INITFN_AUTO_RESULT_TYPE(
        WriteHandler, void (boost::system::error_code, std::size_t)
    )
    async_send_instrution_data(
        u_int16_t instruction,
        const std::vector<byte>& buffers,
        socket_base::message_flags flags,
        BOOST_ASIO_MOVE_ARG(WriteHandler) handler
            BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type)
    ) {
        std::vector<byte> data = pack_(instruction, buffers);
        return async_send(boost::asio::buffer(data), flags, handler);
    }

    template <
        typename MutableBufferSequence,
        BOOST_ASIO_COMPLETION_TOKEN_FOR(
            void (boost::system::error_code, std::size_t)
        ) ReadHandler BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)
    >
    BOOST_ASIO_INITFN_AUTO_RESULT_TYPE(
        ReadHandler, void (boost::system::error_code, std::size_t)
    )
    async_receive_instrution_data(
        const MutableBufferSequence& buffers,
        socket_base::message_flags flags,
        BOOST_ASIO_MOVE_ARG(ReadHandler) handler
            BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type)
    ) {
        return async_initiate<ReadHandler,
        void (boost::system::error_code, std::size_t)>(
            initiate_async_receive(this), handler, buffers, flags);
    }

protected:

    std::vector<byte> pack_(
        u_int16_t instruction,
        const std::vector<byte>& data
    );

    std::pair<uint16_t, std::vector<byte>> unpack(
        const std::vector<byte>& idtcp_raw
    );

};

} // namespace net
} // namespace mixi
