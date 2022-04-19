#pragma once
#include "pch.h"
#include "gui/imgui/window.h"

namespace mixi
{
namespace gobang
{

class WindowDashboard : public imgui::Window
{
public:

    WindowDashboard();
    ~WindowDashboard();

    void content() override;

private:

};

} // namespace gobang
} // namespace mixi
