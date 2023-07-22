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

    boost::asio::io_context net_ctx_;
    boost::asio::executor_work_guard<
        boost::asio::io_context::executor_type
    > net_work_guard_;
    std::thread net_thread_;
    ServerGameRoom server_game_room_;
    boost::signals2::connection join_room_connection_;

    std::atomic<bool> is_gaming_;

    void render_game_room_();
    void render_home_window_();

    void on_join_room(ServerGameRoom::JoinRoomState state);

};

} // namespace gobang
} // namespace mixi