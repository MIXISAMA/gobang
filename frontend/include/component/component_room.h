#pragma once
#include "pch.h"
#include "mixi/gui/component.h"

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

    ComponentRoom(gui::Context& context, ServerGameRoom& server);

protected:

    void content() override;

private:

    ServerGameRoom& server_;

    WindowGame window_game_;
    WindowChat window_chat_;
    WindowDashboard window_dashboard_;
    ModalConfirmRegret modal_confirm_regret_;
    ModalConfirmTie    modal_confirm_tie_;

};
    
} // namespace gobang
} // namespace mixi
