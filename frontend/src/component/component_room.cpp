#include "component/component_room.h"

namespace mixi
{
namespace gobang
{

ComponentRoom::ComponentRoom(gui::Context& context) :
    gui::Component<ComponentRoom>(context),
    io_context_game_room_(),
    server_game_room_(io_context_game_room_),
    window_game_(context),
    window_chat_(context),
    window_dashboard_(context, game_room_)
{
    boost::thread thread_game_room([this]{io_context_game_room_.run();});
    thread_game_room.detach();
}

ComponentRoom::~ComponentRoom()
{
    io_context_game_room_.stop();
}

void ComponentRoom::content()
{
    RtqReader<msg_t> messages(server_game_room_.messages());
    if (messages.locked() && !messages.empty()) {
        auto [nickname, time, text] = messages.pop();
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
