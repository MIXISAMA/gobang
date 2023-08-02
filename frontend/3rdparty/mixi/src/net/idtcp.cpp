#include "mixi/net/idtcp.h"
#include "mixi/core/log.h"

namespace mixi
{
namespace net
{

IdtcpSocket::IdtcpSocket(
    boost::asio::io_context& io_context,
    const boost::asio::ip::tcp::endpoint local_endpoint
) :
    boost::asio::ip::tcp::socket(io_context, local_endpoint)
{

}

void IdtcpSocket::async_send_instrution_data(
    uint16_t instruction,
    const std::vector<std::byte>& buffers,
    const boost::function<void(const boost::system::error_code&, std::size_t)>& handler
) {
    uint16_t send_buf_bytes;
    Pack_(instruction, buffers, send_buffer_, send_buf_bytes);
    async_send(boost::asio::buffer(send_buffer_, send_buf_bytes), handler);
}

void IdtcpSocket::async_receive_instrution_data(
    const std::function<
        void(uint16_t, const std::vector<std::byte>&)
    >& handler
) {
    async_receive(boost::asio::buffer(buffer_, 2),
        boost::bind(&IdtcpSocket::handle_receive_, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred,
            handler
        )
    );
}

boost::asio::awaitable<void>
IdtcpSocket::send_instrution_data(
    uint16_t instruction,
    const std::vector<std::byte>& buffers
) const {
    uint16_t send_buf_bytes;
    Pack_(instruction, buffers, send_buffer_, send_buf_bytes);
    co_await const_cast<IdtcpSocket*>(this)->async_send(
        boost::asio::buffer(send_buffer_, send_buf_bytes),
        boost::asio::use_awaitable
    );
    std::ostringstream oss;
    oss << "Sent     [" << instruction << "] Length = " << send_buf_bytes;
    Log::Info(oss.str());
}

boost::asio::awaitable<std::pair<uint16_t, std::vector<std::byte>>>
IdtcpSocket::receive_instrution_data()
{    
    uint16_t bytes_buffer = 0;
    while (bytes_buffer < 2) {
        int bytes_transferred = co_await async_read_some(
            boost::asio::buffer(buffer_ + bytes_buffer, 2 - bytes_buffer),
            boost::asio::use_awaitable
        );
        bytes_buffer += bytes_transferred;
    }
    
    uint16_t bytes_package = (uint16_t)buffer_[1] << 8 | (uint16_t)buffer_[0];
    while (bytes_buffer < bytes_package) {
        int bytes_transferred = co_await async_read_some(
            boost::asio::buffer(buffer_ + bytes_buffer, bytes_package - bytes_buffer),
            boost::asio::use_awaitable
        );
        bytes_buffer += bytes_transferred;
    }

    std::pair<uint16_t, std::vector<std::byte>> id;
    Unpack_(buffer_, bytes_package, id.first, id.second);
    co_return id;
}

void IdtcpSocket::Pack_(
    uint16_t instruction,
    const std::vector<std::byte>& data,
    std::byte* idtcp_raw,
    uint16_t& idtcp_raw_bytes
) {
    if (data.size() > 65532) {
        throw std::runtime_error("data is too large");
    }
    idtcp_raw_bytes = (uint16_t)data.size() + 4;
    memcpy(idtcp_raw + 0, &idtcp_raw_bytes, 2);
    memcpy(idtcp_raw + 2, &instruction, 2);
    memcpy(idtcp_raw + 4, data.data(), data.size());
}

void IdtcpSocket::Unpack_(
    const std::byte* idtcp_raw,
    uint16_t idtcp_raw_bytes,
    uint16_t& instruction,
    std::vector<std::byte>& data
) {
    data.resize(idtcp_raw_bytes - 4);
    instruction = (uint16_t)idtcp_raw[3] << 8 | (uint16_t)idtcp_raw[2];
    memcpy(data.data(), &idtcp_raw[4], idtcp_raw_bytes - 4);
}

void IdtcpSocket::handle_receive_(
    const boost::system::error_code& error,
    std::size_t bytes_transferred,
    const std::function<
        void(uint16_t, const std::vector<std::byte>&)
    >& handler
) {
    if (error) {
        std::ostringstream oss;
        oss << "IDTCP Client Handle Receive Error: "
            << error.message();
        Log::Error(oss.str());
        return;
    }

    if (bytes_transferred == 1) {
        read_some(boost::asio::buffer(buffer_ + 1, 1));
    }

    uint16_t package_bytes_ = *(uint16_t*)buffer_;
    uint16_t data_bytes = package_bytes_ - 4;

    for (uint16_t buffer_bytes_ = 2; buffer_bytes_ != package_bytes_;) {
        buffer_bytes_ += read_some(
            boost::asio::buffer(
                &buffer_[buffer_bytes_],
                package_bytes_ - buffer_bytes_
            )
        );
    }

    uint16_t instruction;
    std::vector<std::byte> data;
    Unpack_(buffer_, package_bytes_, instruction, data);
    handler(instruction, data);
}


IdtcpClient::IdtcpClient(
    boost::asio::io_context& io_context,
    std::initializer_list<Distribute> distribution,
    const std::function<void(void)>& connected_callback,
    const std::function<void(void)>& disconnected_callback
) : 
    socket_(io_context, boost::asio::ip::tcp::endpoint()),
    distribution_(distribution),
    on_connected_(connected_callback),
    on_disconnected_(disconnected_callback)
{

}

IdtcpClient::~IdtcpClient()
{
    socket_.close();
}

boost::asio::awaitable<void>
IdtcpClient::connect(const boost::asio::ip::tcp::endpoint& remote_endpoint)
{
    try {
        co_await socket_.async_connect(
            remote_endpoint,
            boost::asio::use_awaitable
        );
    }
    catch(...) {
        on_disconnected_();
        socket_.close();
        co_return;
    }
    on_connected_();
    co_await start_receive_();
    on_disconnected_();
    socket_.close();
}

boost::asio::awaitable<void>
IdtcpClient::send(
    uint16_t instruction,
    const std::vector<std::byte> buffers
) const {
    co_await socket_.send_instrution_data(instruction, buffers);
}

bool IdtcpClient::connected() const
{
    return connected_;
}

boost::asio::awaitable<void> IdtcpClient::start_receive_()
{
    while (socket_.is_open()) {
        std::pair<uint16_t, std::vector<std::byte>> id;
        try {
            id = co_await socket_.receive_instrution_data();
        } catch (...) {
            break;
        }
        std::ostringstream oss;
        oss << "Received [" << id.first << "] Length = " << id.second.size() + 4;
        Log::Info(oss.str());
        if (id.first >= distribution_.size()) {
            Log::Error("Instruction out of distribution size!");
        } else {
            distribution_[id.first](id.second);
        }
    }
}


} // namespace net
} // namespace mixi
