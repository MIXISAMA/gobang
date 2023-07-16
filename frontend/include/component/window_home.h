#pragma once
#include "pch.h"

#include "gui/component.h"

#include "component/modal_room_search.h"
#include "component/modal_room_create.h"

#include "server/server_game_room.h"

namespace mixi {
namespace gobang {

class WindowHome : public gui::Window
{
public:

    WindowHome(gui::Context& context, ServerGameRoom& server_game_room);
    ~WindowHome();

    void content() override;

    const ModalRoomCreate& modal_room_create() const;
    const ModalRoomSearch& modal_room_search() const;

private:

    ModalRoomCreate modal_room_create_;
    ModalRoomSearch modal_room_search_;

};

} // namespace gobang
} // namespace mixi