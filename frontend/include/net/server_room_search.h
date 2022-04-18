#pragma once

#include "pch.h"
#include "game/room.h"

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

    ServerRoomSearch();
    ~ServerRoomSearch();

    void search_room(const boost::asio::ip::udp::endpoint &endpoint);

    const std::vector<ConciseRoom>& rooms();

private:

    void start_receive_();
    void handle_receive_(
        const boost::system::error_code& error
        // std::size_t /*bytes_transferred*/
    );
    void handle_send_(const boost::system::error_code& error);

    boost::asio::ip::udp::socket socket_;
    std::vector<std::byte>  recv_buffer_;
    boost::asio::ip::udp::endpoint sender_endpoint_;

    
    std::set<ConciseRoom> room_set_;
    std::shared_mutex room_set_mutex_;

    std::atomic_bool has_new_rooms_;
    std::vector<ConciseRoom> rooms_;
    

};

} // namespace gobang
} // namespace mixi
