#pragma once

#include "pch.h"
#include "mixi/gui/component.h"

#include "component/popup_emoji.h"
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

private:

    PopupEmoji popup_emoji_;

    ServerRoomSearch server_room_search_;
    ServerGameRoom& server_game_room_;

    boost::signals2::connection join_room_connection_;

    std::set<std::shared_ptr<ConciseRoom>> rooms_;
    std::shared_ptr<ConciseRoom> selected_room_;

    boost::asio::ip::address_v4::bytes_type search_ip_;
    u_int16_t search_port_;
    char username_[64];
    char password_[64];

    void update_rooms_();
    void on_search_();
    void on_join_room_(ServerGameRoom::JoinRoomState state);
    void on_select_password_emoji_(const char* emoji);
    void on_backspace_password_emoji_(void);

    std::atomic<const char*> hint_;

    static const char* Hint_Search_;
    static const char* Hint_Joining_;
    static const char* Hint_Failed_;

};

} // namespace gobang
} // namespace mixi
