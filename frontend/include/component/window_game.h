#pragma once
#include "pch.h"
#include "gui/imgui/component/window.h"

namespace mixi
{
namespace gobang
{

class WindowGame : public imgui::Window
{
public:

    WindowGame(imgui::Context& context);
    ~WindowGame();

    void content() override;

private:

};

} // namespace gobang
} // namespace mixi
