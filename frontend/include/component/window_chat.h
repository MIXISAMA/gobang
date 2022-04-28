#pragma once
#include "pch.h"
#include "gui/imgui/component/window.h"

namespace mixi
{
namespace gobang
{

class WindowChat : public imgui::Window
{
public:

    WindowChat(imgui::Context& context);
    ~WindowChat();

    void add_message(const std::string& message);
    
    bool has_input();
    const std::string& fetch_input();

    void content() override;

private:

    std::deque<std::string> messages_;

    char input_buffer_[256];
    std::string input_;
    bool has_input_;

};

} // namespace gobang
} // namespace mixi
