#include "component/window_home.h"

namespace mixi {
namespace gobang {

WindowHome::WindowHome(
    imgui::Context& context,
    ServerGameRoom& server_game_room
) :
    imgui::Window(context, gettext("Home")),
    server_game_room_(server_game_room),
    room_create_modal_(context),
    room_search_modal_(context, server_game_room_)
{

}

WindowHome::~WindowHome()
{

}

void WindowHome::content()
{
    ImVec2 window_size = ImGui::GetWindowSize();

    ImVec2 text_size = ImGui::CalcTextSize(gettext("Gobang"));
    ImVec2 text_pos = ImVec2((window_size.x - text_size.x) * 0.5f, window_size.y * 0.3f);
    ImGui::SetCursorPos(text_pos);
    ImGui::Text("%s", gettext("Gobang"));

    ImVec2 button_pos = ImVec2(window_size.x * 0.5f - 160.0f, window_size.y * 0.6f - 40.0f);
    ImGui::SetCursorPos(button_pos);
    if (ImGui::Button(gettext("Create Room"), ImVec2(160, 80))) {
        room_create_modal_.open();
    }
    ImGui::SameLine();
    if (ImGui::Button(gettext("Search Room"), ImVec2(160, 80))) {
        room_search_modal_.open();
    }

    room_create_modal_.render();

    room_search_modal_.render();

}

bool WindowHome::join_done()
{
    return room_search_modal_.join_done();
}

} // namespace gobang
} // namespace mixi