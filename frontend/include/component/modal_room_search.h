#pragma once

#include "pch.h"
#include "gui/imgui/popup.h"

#include "net/server_room_search.h"

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

private:

    void on_search_();

    void on_join_as_player_();
    void on_join_as_onlooker_();

    ServerRoomSearch server_room_search_;

    int item_current_idx_;

    boost::asio::ip::address_v4::bytes_type search_ip_;
    u_int16_t search_port_;

};

} // namespace gobang
} // namespace mixi
