#include "component/component_room.h"

namespace mixi
{
namespace gobang
{

ComponentRoom::ComponentRoom(ServerGameRoom& server_game_room) :
    server_game_room_(server_game_room),
    window_dashboard_(server_game_room.game_room())
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

} // namespace gobang
} // namespace mixi
