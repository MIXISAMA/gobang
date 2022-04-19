#pragma once
#include "pch.h"
#include "gui/imgui/window.h"

namespace mixi
{
namespace gobang
{

class WindowGame : public imgui::Window
{
public:

    WindowGame();
    ~WindowGame();

    void content() override;

private:

};

} // namespace gobang
} // namespace mixi
