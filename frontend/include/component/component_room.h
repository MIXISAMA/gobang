#pragma once
#include "pch.h"
#include "gui/component.h"

#include "server/server_game_room.h"

#include "component/window_dashboard.h"
#include "component/window_game.h"
#include "component/window_chat.h"

namespace mixi
{
namespace gobang
{

class ComponentRoom : public gui::Component<ComponentRoom>
{
public:

    ComponentRoom(gui::Context& context);
    ~ComponentRoom();

    void content() override;

    bool leave_done();

private:

    game::Room game_room_;

    boost::asio::io_context io_context_game_room_;
    ServerGameRoom server_game_room_;

    WindowDashboard window_dashboard_;
    WindowGame window_game_;
    WindowChat window_chat_;
};
    
} // namespace gobang
} // namespace mixi
