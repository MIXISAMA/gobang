#include "gui/imgui/window.h"

namespace mixi
{
namespace imgui
{

Window::Window(const std::string &name, ImGuiWindowFlags flags) :
    name_(name),
    flags_(flags),
    p_open_(nullptr)
{

}

void Window::render() {
    if (ImGui::Begin(name_.c_str(), p_open_, flags_)) {
        content();
        ImGui::End();
    }
}


} // namespace imgui
} // namespace mixi
