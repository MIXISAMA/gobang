#include "component/component_room.h"

namespace mixi
{
namespace gobang
{

ComponentRoom::ComponentRoom(gui::Context& context, ServerGameRoom& server) :
    gui::Component<ComponentRoom>(context),
    server_(server),
    window_game_(context, server_.room()),
    window_chat_(context),
    window_dashboard_(context, server_.room()),
    modal_confirm_regret_(context),
    modal_confirm_tie_   (context)
{
    std::byte role = RfbReader<game::Room>(server_.room())->role(server_.username());

    window_game_.role(role);
    window_game_.on_stone(std::bind(&ServerGameRoom::send_player_stone, &server, std::placeholders::_1));

    window_chat_.on_send_message(std::bind(&ServerGameRoom::send_message, &server, std::placeholders::_1));

    window_dashboard_.role(role);
    window_dashboard_.on_leave    (std::bind(&ServerGameRoom::send_leave_room,    &server));
    window_dashboard_.on_regret   (std::bind(&ServerGameRoom::send_player_regret, &server));
    window_dashboard_.on_tie      (std::bind(&ServerGameRoom::send_player_tie,    &server));
    window_dashboard_.on_give_up  (std::bind(&ServerGameRoom::send_give_up,       &server));
    window_dashboard_.on_user_info(std::bind(&ServerGameRoom::send_user_info,     &server, std::placeholders::_1));

    modal_confirm_regret_.on_agree_regret(std::bind(&ServerGameRoom::send_agree_regret, &server, std::placeholders::_1));
    modal_confirm_tie_   .on_agree_tie   (std::bind(&ServerGameRoom::send_agree_tie,    &server, std::placeholders::_1));
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
