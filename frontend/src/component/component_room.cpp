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
    if (server_game_room_.has_message()) {
        auto [nickname, time, text] = server_game_room_.pop_message();
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%T")
            << " [" << nickname << "]:\n" << text;
        window_chat_.add_message(oss.str());
    }
    if (window_chat_.has_input()) {
        const std::string& text = window_chat_.fetch_input();
        server_game_room_.send_message(text);
    }

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
