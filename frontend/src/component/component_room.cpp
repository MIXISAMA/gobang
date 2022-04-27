#include "component/component_room.h"

namespace mixi
{
namespace gobang
{

ComponentRoom::ComponentRoom(
    imgui::Context& context,
    ServerGameRoom& server_game_room
) :
    imgui::BaseComponent(context),
    server_game_room_(server_game_room),
    window_game_(context),
    window_chat_(context),
    window_dashboard_(context, server_game_room.game_room())
{

}

ComponentRoom::~ComponentRoom()
{

}

void ComponentRoom::content()
{
    window_dashboard_.render();
    window_game_.render();
    window_chat_.render();
}

bool ComponentRoom::leave_done()
{
    if (window_dashboard_.leave()) {
        server_game_room_.leave_room();
        return true;
    }
    return false;
}

} // namespace gobang
} // namespace mixi
