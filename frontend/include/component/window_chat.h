#pragma once
#include "pch.h"
#include "mixi/gui/component.h"
#include "component/popup_emoji.h"

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

protected:

    void content() override;

private:

    std::deque<std::string> messages_;

    char input_buffer_[256];

    PopupEmoji popup_emoji_;

    std::function<void(const std::string&)> on_send_message_;

    void on_selected_emoji_(const char* emoji);

};

} // namespace gobang
} // namespace mixi
