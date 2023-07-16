#include "component/window_home.h"

namespace mixi {
namespace gobang {

WindowHome::WindowHome(gui::Context& context, ServerGameRoom& server_game_room) :
    gui::Window(context, gettext("Home")),
    modal_room_create_(context),
    modal_room_search_(context, server_game_room)
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
        modal_room_create_.open();
    }
    ImGui::SameLine();
    if (ImGui::Button(gettext("Search Room"), ImVec2(160, 80))) {
        modal_room_search_.open();
    }

    modal_room_create_.render();
    modal_room_search_.render();

}

const ModalRoomCreate& WindowHome::modal_room_create() const
{
    return modal_room_create_;
}

const ModalRoomSearch& WindowHome::modal_room_search() const
{
    return modal_room_search_;
}

} // namespace gobang
} // namespace mixi