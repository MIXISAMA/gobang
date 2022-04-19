#include "net/idtcp.h"

namespace mixi
{
namespace net
{

// std::vector<byte> IdtcpPack(
//     u_int16_t instruction,
//     const std::vector<byte>& data
// ) {
//     if (data.size() > 65532) {
//         throw std::runtime_error("data is too large");
//     }
//     u_int16_t length = data.size() + 4;
//     std::vector<byte> idtcp_raw(length);
//     *((u_int16_t*)idtcp_raw.data() + 0) = length;
//     *((u_int16_t*)idtcp_raw.data() + 1) = instruction;
//     std::copy(data.begin(), data.end(), idtcp_raw.at(4));
//     return idtcp_raw;
// }



IdtcpSocket::IdtcpSocket(
    boost::asio::io_context& io_context,
    const boost::asio::ip::tcp::endpoint local_endpoint
) :
    boost::asio::ip::tcp::socket(io_context, local_endpoint)
{

}


std::vector<byte> IdtcpSocket::pack_(
    u_int16_t instruction,
    const std::vector<byte>& data
) {
    if (data.size() > 65532) {
        throw std::runtime_error("data is too large");
    }
    u_int16_t length = data.size() + 4;
    std::vector<byte> idtcp_raw(length);
    memcpy(idtcp_raw.data() + 0, &length, 2);
    memcpy(idtcp_raw.data() + 2, &instruction, 2);
    memcpy(idtcp_raw.data() + 4, data.data(), data.size());
    return idtcp_raw;
}

std::pair<uint16_t, std::vector<byte>> unpack_(
    const std::vector<byte>& idtcp_raw
) {

}


} // namespace net
} // namespace mixi
