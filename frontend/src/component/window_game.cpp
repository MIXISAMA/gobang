#include "component/window_game.h"


namespace mixi {
namespace gobang {

WindowGame::WindowGame(imgui::Context& context) :
    imgui::Window(context, gettext("Gobang Game"))
{

}

WindowGame::~WindowGame()
{

}

void WindowGame::content()
{

}

} // namespace gobang
} // namespace mixi