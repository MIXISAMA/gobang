#pragma once
#include "pch.h"

#include "gui/imgui/component/component.h"

#include "component/window_home.h"
#include "component/component_room.h"

#include "server/server_game_room.h"

namespace mixi {
namespace gobang {

class ComponentRoot : public imgui::BaseComponent
{
public:

    ComponentRoot(imgui::Context& context);
    ~ComponentRoot();

    void content() override;

private:

    WindowHome::Ptr window_home_;
    ComponentRoom::Ptr component_room_;

    ServerGameRoom server_game_room_;

    bool gaming_;

};

} // namespace gobang
} // namespace mixi