#pragma once

#include "pch.h"
#include "gui/imgui/popup.h"

#include "game/room.h"
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

    std::vector<Room> rooms_;

    ServerRoomSearch server_room_search_;

};

} // namespace gobang
} // namespace mixi
