#pragma once
#include "pch.h"

#include "gui/component.h"

#include "component/window_home.h"
#include "component/component_room.h"

#include "server/server_game_room.h"

namespace mixi {
namespace gobang {

class ComponentRoot : public gui::Component<ComponentRoot>
{
public:

    ComponentRoot(gui::Context& context);
    ~ComponentRoot();

    void content() override;

private:

    std::unique_ptr<WindowHome>    window_home_;
    std::unique_ptr<ComponentRoom> component_room_;

    boost::asio::io_context io_context_;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_;
    ServerGameRoom server_game_room_;

    bool gaming_;

    void render_game_room_();
    void render_home_window_();

};

} // namespace gobang
} // namespace mixi