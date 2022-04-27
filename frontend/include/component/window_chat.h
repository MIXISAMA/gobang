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

    void content() override;

private:

};

} // namespace gobang
} // namespace mixi
