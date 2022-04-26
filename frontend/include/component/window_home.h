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

    using Ptr = std::shared_ptr<WindowHome>;

    WindowHome(ServerGameRoom& server_game_room);
    ~WindowHome();

    void content() override;

    bool join_done();

private:

    ServerGameRoom& server_game_room_;

    ModalRoomCreate room_create_modal_;
    ModalRoomSearch room_search_modal_;

};

} // namespace gobang
} // namespace mixi