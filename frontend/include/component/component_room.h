#pragma once
#include "pch.h"
#include "gui/imgui/component/component.h"

#include "server/server_game_room.h"

#include "component/window_dashboard.h"
#include "component/window_game.h"
#include "component/window_chat.h"

namespace mixi
{
namespace gobang
{

class ComponentRoom : public imgui::BaseComponent
{
public:

    using Ptr = std::shared_ptr<ComponentRoom>;

    ComponentRoom(
        imgui::Context& context,
        ServerGameRoom& server_game_room
    );
    ~ComponentRoom();

    void content() override;

    bool leave_done();

private:

    WindowDashboard window_dashboard_;
    WindowGame window_game_;
    WindowChat window_chat_;

    ServerGameRoom& server_game_room_;

};
    
} // namespace gobang
} // namespace mixi
