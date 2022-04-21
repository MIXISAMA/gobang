#include "gui/imgui/popup.h"

namespace mixi
{
namespace imgui
{

long long PopupModal::Id_ = 0;

PopupModal::PopupModal(const std::string &name, ImGuiWindowFlags flags) :
    name_(name),
    flags_(flags),
    should_close_(false)
{

}

void PopupModal::open()
{
    ImGui::OpenPopup(name_.c_str());
}

void PopupModal::close()
{
    should_close_ = true;
}

bool PopupModal::is_opening()
{
    return ImGui::IsPopupOpen(name_.c_str());
}

void PopupModal::pre_render()
{
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
}

void PopupModal::render() {
    pre_render();
    bool p_open = true;
    if (ImGui::BeginPopupModal(name_.c_str(), &p_open, flags_)) {
        if (should_close_) {
            should_close_ = false;
            ImGui::CloseCurrentPopup();
        } else {
            content();
        }
        ImGui::EndPopup();
    }
}

} // namespace imgui
} // namespace mixi
