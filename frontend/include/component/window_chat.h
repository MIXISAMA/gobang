#pragma once
#include "pch.h"
#include "mixi/gui/component.h"

namespace mixi
{
namespace gobang
{

class WindowChat : public gui::Window
{
public:

    WindowChat(gui::Context& context);
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
