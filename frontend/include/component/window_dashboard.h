#pragma once
#include "pch.h"
#include "gui/imgui/window.h"
#include "game/room.h"

namespace mixi
{
namespace gobang
{

class WindowDashboard : public imgui::Window
{
public:

    WindowDashboard(const game::Room& room);
    ~WindowDashboard();

    void content() override;

private:

    const game::Room& room_;

};

} // namespace gobang
} // namespace mixi
