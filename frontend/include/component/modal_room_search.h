#pragma once

#include "pch.h"
#include "gui/component.h"

#include "server/server_room_search.h"
#include "server/server_game_room.h"

namespace mixi
{
namespace gobang
{

class ModalRoomSearch : public gui::PopupModal
{

public:

    ModalRoomSearch(gui::Context& context, ServerGameRoom& server_game_room);
    ~ModalRoomSearch();

    void content() override;

    // bool should_join_room() const;
    // std::pair<ConciseRoom, bool> info_join_room() const;

private:

    ServerRoomSearch server_room_search_;

    ServerGameRoom& server_game_room_;

    std::vector<ConciseRoom> rooms_;
    int item_current_idx_;

    boost::asio::ip::address_v4::bytes_type search_ip_;
    u_int16_t search_port_;
    char username_[64];
    char password_[64];

    void update_rooms_();
    void on_search_();

    // bool should_join_room_;
    // bool as_a_player_;

    

    const char* hint_;

    static const char* Hint_Search_;
    static const char* Hint_Joining_;
    static const char* Hint_Failed_;

};

} // namespace gobang
} // namespace mixi
