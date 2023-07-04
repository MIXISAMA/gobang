#include "gui/component.h"

namespace mixi
{
namespace gui
{

NameAble::NameAble(const std::string& n) :
    name_(n)
{
    
}

void NameAble::name(const std::string& n)
{
    name_ = n;
}

const std::string& NameAble::name()
{
    return name_;
}


FlagsAble::FlagsAble(ImGuiWindowFlags f) :
    flags_(f)
{

}

void FlagsAble::flags(ImGuiWindowFlags f)
{
    flags_ = f;
}

ImGuiWindowFlags FlagsAble::flags()
{
    return flags_;
}


PopupModal::PopupModal(Context& context, const std::string name, ImGuiWindowFlags flag) :
    Component(context),
    NameAble(name),
    FlagsAble(flag),
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

void PopupModal::render()
{
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



Window::Window(Context& context, const std::string name) :
    Component(context),
    NameAble(name),
    p_open_(nullptr)
{

}

void Window::render() {
    if (ImGui::Begin(name_.c_str(), p_open_, flags_)) {
        content();
    }
    ImGui::End();
}


} // namespace gui
} // namespace mixi
