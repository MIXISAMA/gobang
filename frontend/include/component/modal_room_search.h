#pragma once

#include "pch.h"
#include "gui/imgui/popup.h"

#include "net/server_room_search.h"
#include "net/server_game_room.h"

namespace mixi
{
namespace gobang
{

class ModalRoomSearch : public imgui::PopupModal
{

public:

    ModalRoomSearch();
    ~ModalRoomSearch();

    void content() override;

    bool join_done();
    std::shared_ptr<ServerGameRoom> server_game_room();

private:

    ServerRoomSearch server_room_search_;
    const std::vector<ConciseRoom>& rooms_;
    int item_current_idx_;

    boost::asio::ip::address_v4::bytes_type search_ip_;
    u_int16_t search_port_;
    char player_name_[64];

    bool join_done_;
    std::shared_ptr<ServerGameRoom> server_game_room_;

    void on_search_();

    void on_click_join_as_player_();
    void on_click_join_as_onlooker_();

    static const char* Hint_Search_;
    static const char* Hint_Joining_;
    static const char* Hint_Failed_;

};

} // namespace gobang
} // namespace mixi
