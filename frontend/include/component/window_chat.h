#pragma once
#include "pch.h"
#include "gui/imgui/window.h"

namespace mixi
{
namespace gobang
{

class WindowChat : public imgui::Window
{
public:

    WindowChat();
    ~WindowChat();

    void content() override;

private:

};

} // namespace gobang
} // namespace mixi
