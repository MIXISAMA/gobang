#include "component/window_home.h"

namespace mixi {
namespace gobang {

WindowHome::WindowHome() :
    imgui::Window(gettext("Home")),
    room_create_modal_(nullptr),
    room_search_modal_(nullptr)
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
    ImGui::Text(gettext("Gobang"));

    ImVec2 button_pos = ImVec2(window_size.x * 0.5f - 160.0f, window_size.y * 0.6f - 40.0f);
    ImGui::SetCursorPos(button_pos);
    if (ImGui::Button(gettext("Create Room"), ImVec2(160, 80))) {
        room_create_modal_ = std::shared_ptr<ModalRoomCreate>(new ModalRoomCreate());
        room_create_modal_->open();
    }
    ImGui::SameLine();
    if (ImGui::Button(gettext("Search Room"), ImVec2(160, 80))) {
        room_search_modal_ = std::shared_ptr<ModalRoomSearch>(new ModalRoomSearch());
        room_search_modal_->open();
    }

    if (room_create_modal_.get() != nullptr) {
        room_create_modal_->render();
    }

    if (room_search_modal_.get() != nullptr) {
        room_search_modal_->render();
        if (room_search_modal_->join_done()) {
            room_create_modal_->close();
        }
    }

}

} // namespace gobang
} // namespace mixi