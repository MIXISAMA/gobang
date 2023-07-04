#include "net/idtcp.h"
#include "core/log.h"

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
    u_int16_t instruction,
    const std::vector<std::byte>& buffers,
    const boost::function<void(const boost::system::error_code&, std::size_t)>& handler
) {
    u_int16_t send_buf_bytes;
    pack_(instruction, buffers, send_buffer_, send_buf_bytes);
    async_send(boost::asio::buffer(send_buffer_, send_buf_bytes), handler);
}

void IdtcpSocket::async_receive_instrution_data(
    const std::function<
        void(u_int16_t, const std::vector<std::byte>&)
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
    u_int16_t instruction,
    const std::vector<std::byte>& buffers
) {
    u_int16_t send_buf_bytes;
    pack_(instruction, buffers, send_buffer_, send_buf_bytes);
    co_await async_send(
        boost::asio::buffer(send_buffer_, send_buf_bytes),
        boost::asio::use_awaitable
    );
}

boost::asio::awaitable<std::pair<u_int16_t, std::vector<std::byte>>>
IdtcpSocket::receive_instrution_data()
{    
    u_int16_t bytes_buffer = 0;
    while (bytes_buffer < 2) {
        int bytes_transferred = co_await async_read_some(
            boost::asio::buffer(buffer_, 2 - bytes_buffer),
            boost::asio::use_awaitable
        );
        bytes_buffer += bytes_transferred;
    }

    u_int16_t bytes_package = *(u_int16_t*)buffer_;
    while (bytes_buffer < bytes_package) {
        int bytes_transferred = co_await async_read_some(
            boost::asio::buffer(buffer_, bytes_package - bytes_buffer),
            boost::asio::use_awaitable
        );
        bytes_buffer += bytes_transferred;
    }

    std::pair<u_int16_t, std::vector<std::byte>> id;
    unpack_(buffer_, bytes_package, id.first, id.second);
    co_return id;
}

void IdtcpSocket::pack_(
    u_int16_t instruction,
    const std::vector<std::byte>& data,
    std::byte* idtcp_raw,
    u_int16_t& idtcp_raw_bytes
) {
    if (data.size() > 65532) {
        throw std::runtime_error("data is too large");
    }
    idtcp_raw_bytes = (u_int16_t)data.size() + 4;
    memcpy(idtcp_raw + 0, &idtcp_raw_bytes, 2);
    memcpy(idtcp_raw + 2, &instruction, 2);
    memcpy(idtcp_raw + 4, data.data(), data.size());
}

void IdtcpSocket::unpack_(
    const std::byte* idtcp_raw,
    u_int16_t idtcp_raw_bytes,
    u_int16_t& instruction,
    std::vector<std::byte>& data
) {
    data.resize(idtcp_raw_bytes - 4);
    instruction = *(u_int16_t*)&buffer_[2];
    memcpy(data.data(), &buffer_[4], idtcp_raw_bytes - 4);
}

void IdtcpSocket::handle_receive_(
    const boost::system::error_code& error,
    std::size_t bytes_transferred,
    const std::function<
        void(u_int16_t, const std::vector<std::byte>&)
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

    u_int16_t package_bytes_ = *(u_int16_t*)buffer_;
    u_int16_t data_bytes = package_bytes_ - 4;

    for (u_int16_t buffer_bytes_ = 2; buffer_bytes_ != package_bytes_;) {
        buffer_bytes_ += read_some(
            boost::asio::buffer(
                &buffer_[buffer_bytes_],
                package_bytes_ - buffer_bytes_
            )
        );
    }

    u_int16_t instruction;
    std::vector<std::byte> data;
    unpack_(buffer_, package_bytes_, instruction, data);
    handler(instruction, data);
}


IdtcpClient::IdtcpClient(
    boost::asio::io_context& io_context,
    const Distribute& distribute
) : 
    socket_(io_context, boost::asio::ip::tcp::endpoint()),
    distribute_(distribute)
{

}

IdtcpClient::~IdtcpClient()
{
    socket_.close();
}

boost::asio::awaitable<void>
IdtcpClient::connect(const boost::asio::ip::tcp::endpoint& remote_endpoint)
{
    co_await socket_.async_connect(
        remote_endpoint,
        boost::asio::use_awaitable
    );
    boost::asio::co_spawn(
        socket_.get_executor(),
        start_receive_(),
        boost::asio::detached
    );
}

boost::asio::awaitable<void>
IdtcpClient::send(
    u_int16_t instruction,
    const std::vector<std::byte>& buffers
) {
    co_await socket_.send_instrution_data(instruction, buffers);
    std::ostringstream oss;
    oss << "IDTCP Client sent [" << instruction << "]";
    Log::Info(oss.str());
}

boost::asio::awaitable<void> IdtcpClient::start_receive_()
{
    while (socket_.is_open()) {
        auto [instruction, data] = co_await socket_.receive_instrution_data();
        std::ostringstream oss;
        oss << "IDTCP Client Received [" << instruction << "]";
        Log::Info(oss.str());
        if (instruction >= distribute_.size()) {
            Log::Error("Instruction out of distribute size!");
        } else {
            distribute_[instruction](data);
        }
    }
}


} // namespace net
} // namespace mixi
