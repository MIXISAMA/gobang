#include "component/window_chat.h"


namespace mixi {
namespace gobang {

WindowChat::WindowChat(gui::Context& context) :
    gui::Window(context, gettext("Chat")),
    on_send_message_([](const std::string&){}),
    popup_emoji_(context)
{
    input_buffer_[0] = '\0';
    popup_emoji_.on_selected(std::bind(&WindowChat::on_selected_emoji_, this, std::placeholders::_1));
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

    if (ImGui::Button("😃")) {
        popup_emoji_.open();
    }
    popup_emoji_.render();

    ImGui::SameLine();
    if (ImGui::InputText(
            "Enter",
            input_buffer_,
            sizeof(input_buffer_),
            ImGuiInputTextFlags_EnterReturnsTrue
    )) {
        on_send_message_(input_buffer_);
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

void WindowChat::on_send_message(const std::function<void(const std::string&)>& f)
{
    on_send_message_ = f;
}

void WindowChat::on_selected_emoji_(const char* emoji)
{
    if (std::strlen(input_buffer_) >= std::min(((int)sizeof(input_buffer_)) - 5, 6 * 4)) {
        return;
    }
    std::strcat(input_buffer_, emoji);
}

} // namespace gobang
} // namespace mixi