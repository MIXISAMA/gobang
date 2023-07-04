#include "gui/application.h"

#include "core/log.h"

namespace mixi
{
namespace gui
{


Context::Context(GlfwWindow& glfw_window) :
    glfw_window_(glfw_window)
{

}




Application::Application(const char* app_name, int weight, int height) :
    glfw_window_(app_name, weight, height),
    imgui_(glfw_window_.glfw_window()),
    io_context_(),
    app_context_(glfw_window_)
{

}

Application::~Application()
{

}

Context& Application::context()
{
    return app_context_;
}


void Application::run(BaseComponent* main_window, int fps_limit)
{
    boost::asio::co_spawn(io_context_, loop_rendering_(main_window, fps_limit), boost::asio::detached);
    io_context_.run();
}

boost::asio::awaitable<void> Application::loop_rendering_(BaseComponent* window, int fps_limit)
{
    boost::asio::steady_timer timer_render(io_context_);
    while (!glfw_window_.should_close()) {
        timer_render.expires_after(boost::asio::chrono::milliseconds(1000 / fps_limit));

        glfw_window_.pre_render();
        imgui_.pre_render();
        window->render();
        imgui_.post_render();
        glfw_window_.post_render();
        
        co_await timer_render.async_wait(boost::asio::use_awaitable);
    }
    io_context_.stop();
}


} // namespace gui
} // namespace mixi

