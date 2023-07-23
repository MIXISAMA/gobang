#pragma once

#include "pch.h"
#include "mixi/core/lock.h"

namespace mixi
{
namespace gobang
{

struct ConciseRoom
{
    std::string version;
    std::string server_name;
    uint8_t     room_id;
    std::string room_name;
    bool        is_playing;
    std::string black_player;
    std::string white_player;
    uint8_t     users;
    uint8_t     max_users;

    boost::asio::ip::tcp::endpoint endpoint;

    bool operator < (const ConciseRoom& other) const;
};


class ServerRoomSearch
{

public:

    ServerRoomSearch(boost::asio::io_context& io_context);
    ~ServerRoomSearch();

    void search_room(const boost::asio::ip::udp::endpoint &endpoint);

    ReadTryQueue<ConciseRoom>& rooms();

private:

    boost::asio::io_context& io_context_;
    boost::asio::ip::udp::socket socket_;

    ReadTryQueue<ConciseRoom> rooms_;

    boost::asio::awaitable<void> send_search_room_(const boost::asio::ip::udp::endpoint endpoint);
    boost::asio::awaitable<void> receive_();

};

} // namespace gobang
} // namespace mixi
