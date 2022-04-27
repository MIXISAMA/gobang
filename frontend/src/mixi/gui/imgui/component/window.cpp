#include "gui/imgui/component/window.h"

namespace mixi
{
namespace imgui
{

Window::Window(
    Context& context,
    const std::string &name,
    ImGuiWindowFlags flags
) :
    BaseComponent(context),
    name_(name),
    flags_(flags),
    p_open_(nullptr)
{

}

void Window::render() {
    if (ImGui::Begin(name_.c_str(), p_open_, flags_)) {
        content();
    }
    ImGui::End();
}


} // namespace imgui
} // namespace mixi
