#pragma once

#include "pch.h"
#include "gui/imgui/component/popup.h"

#include "server/server_room_search.h"
#include "server/server_game_room.h"

namespace mixi
{
namespace gobang
{

class ModalRoomSearch : public imgui::PopupModal
{

public:

    ModalRoomSearch(
        imgui::Context& context,
        ServerGameRoom& server_game_room
    );
    ~ModalRoomSearch();

    void content() override;

    bool join_done();

private:

    ServerRoomSearch server_room_search_;
    ServerGameRoom& server_game_room_;

    const std::vector<ConciseRoom>& rooms_;
    int item_current_idx_;

    boost::asio::ip::address_v4::bytes_type search_ip_;
    u_int16_t search_port_;
    char player_name_[64];

    void on_search_();

    void join_room_(bool is_player);

    const char* hint_;

    static const char* Hint_Search_;
    static const char* Hint_Joining_;
    static const char* Hint_Failed_;

};

} // namespace gobang
} // namespace mixi
