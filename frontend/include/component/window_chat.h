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

    void add_message(const std::string& message);

    void on_send_message(const std::function<void(const std::string&)>& f);

    void content() override;

private:

    std::deque<std::string> messages_;

    char input_buffer_[256];

    std::function<void(const std::string&)> on_send_message_;

};

} // namespace gobang
} // namespace mixi
