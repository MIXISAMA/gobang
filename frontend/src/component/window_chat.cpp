#include "component/window_chat.h"


namespace mixi {
namespace gobang {

WindowChat::WindowChat(gui::Context& context) :
    gui::Window(context, gettext("Chat")),
    has_input_(false)
{
    input_buffer_[0] = '\0';
}

WindowChat::~WindowChat()
{

}

void WindowChat::content()
{
    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

    for (const std::string& msg : messages_) {
        ImGui::Text("%s", msg.c_str());
    }

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();

    if (ImGui::InputText(
            "Enter",
            input_buffer_,
            sizeof(input_buffer_),
            ImGuiInputTextFlags_EnterReturnsTrue
    )) {
        input_.assign(input_buffer_);
        has_input_ = true;
        input_buffer_[0] = '\0';
    }
}

void WindowChat::add_message(const std::string& message)
{
    messages_.push_back(message);
    if (messages_.size() > 100) {
        messages_.pop_front();
    }
}

bool WindowChat::has_input()
{
    return has_input_;
}

const std::string& WindowChat::fetch_input()
{
    has_input_ = false;
    return input_;
}

} // namespace gobang
} // namespace mixi