#pragma once
#include "pch.h"

#include "gui/imgui/window.h"

#include "component/modal_room_search.h"
#include "component/modal_room_create.h"

namespace mixi {
namespace gobang {

class WindowHome : public imgui::Window
{
public:

    WindowHome();
    ~WindowHome();

    void content() override;

private:

    ModalRoomCreate room_create_modal_;
    ModalRoomSearch room_search_modal_;

};

} // namespace gobang
} // namespace mixi