#pragma once
#include "pch.h"

#include "gui/imgui/window.h"

#include "component/modal_room_search.h"
#include "component/modal_room_create.h"

#include "net/server_game_room.h"

namespace mixi {
namespace gobang {

class WindowHome : public imgui::Window
{
public:

    WindowHome(ServerGameRoom& server_game_room);
    ~WindowHome();

    void content() override;

private:

    std::shared_ptr<ModalRoomCreate> room_create_modal_;
    std::shared_ptr<ModalRoomSearch> room_search_modal_;

    ServerGameRoom& server_game_room_;

};

} // namespace gobang
} // namespace mixi