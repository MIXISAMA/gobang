#pragma once

#include "pch.h"

namespace mixi
{
namespace gobang
{

struct ConciseRoom
{
    u_int16_t   id;
    std::string name;
    bool        is_playing;
    std::string player_name[2];
    u_int16_t   onlooker_num;
    u_int16_t   max_onlooker_num;

    boost::asio::ip::tcp::endpoint endpoint;

    bool operator < (const ConciseRoom& room) const;
};


class ServerRoomSearch
{

public:

    ServerRoomSearch(boost::asio::io_context& io_context);
    ~ServerRoomSearch();

    void search_room(const boost::asio::ip::udp::endpoint &endpoint);

    std::vector<ConciseRoom> new_rooms();

private:

    boost::asio::io_context& io_context_;
    boost::asio::ip::udp::socket socket_;

    std::mutex rooms_mutex_;
    std::vector<ConciseRoom> rooms_;

    boost::asio::awaitable<void> send_search_room_(const boost::asio::ip::udp::endpoint endpoint);
    boost::asio::awaitable<void> receive_();

};

} // namespace gobang
} // namespace mixi
