#include "component/window_dashboard.h"


namespace mixi {
namespace gobang {

WindowDashboard::WindowDashboard(const game::Room& room) :
    imgui::Window(gettext("Dashboard")),
    room_(room)
{

}

WindowDashboard::~WindowDashboard()
{

}

void WindowDashboard::content()
{

}

} // namespace gobang
} // namespace mixi
