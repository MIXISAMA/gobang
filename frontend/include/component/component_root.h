#pragma once
#include "pch.h"

#include "gui/imgui/base/component.h"

#include "component/window_home.h"
#include "net/server_game_room.h"

namespace mixi {
namespace gobang {

class ComponentRoot : public imgui::BaseComponent
{
public:

    ComponentRoot();
    ~ComponentRoot();

    void content() override;

private:

    WindowHome::Ptr window_home_;
    ServerGameRoom server_game_room_;

};

} // namespace gobang
} // namespace mixi