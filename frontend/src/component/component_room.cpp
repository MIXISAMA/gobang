#include "component/component_room.h"

namespace mixi
{
namespace gobang
{

ComponentRoom::ComponentRoom(gui::Context& context, ServerGameRoom& server) :
    gui::Component<ComponentRoom>(context),
    server_(server),
    window_game_(context),
    window_chat_(context),
    window_dashboard_(context, server_.room())
{
    RfbReader<game::Room> room(server_.room());
    std::byte role = room->role(server.username());

    window_game_.role(role);
    window_game_.on_stone(std::bind(&ServerGameRoom::send_player_stone, &server, std::placeholders::_1));

    window_chat_.on_send_message(std::bind(&ServerGameRoom::send_message, &server, std::placeholders::_1));

    window_dashboard_.role(role);
    window_dashboard_.on_leave    (std::bind(&ServerGameRoom::send_leave_room,    &server));
    window_dashboard_.on_regret   (std::bind(&ServerGameRoom::send_player_regret, &server));
    window_dashboard_.on_tie      (std::bind(&ServerGameRoom::send_player_tie,    &server));
    window_dashboard_.on_give_up  (std::bind(&ServerGameRoom::send_give_up,       &server));
    window_dashboard_.on_user_info(std::bind(&ServerGameRoom::send_user_info,     &server, std::placeholders::_1));
}

void ComponentRoom::content()
{
    RtqReader<msg_t> messages(server_.messages());
    if (messages.locked() && !messages.empty()) {
        auto [nickname, time, text] = messages.pop();
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%T")
            << " [" << nickname << "]:\n" << text;
        window_chat_.add_message(oss.str());
    }

    window_dashboard_.render();
    window_game_.render();
    window_chat_.render();
}

} // namespace gobang
} // namespace mixi
