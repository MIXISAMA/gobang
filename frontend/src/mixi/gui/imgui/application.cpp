#include "gui/imgui/application.h"

#include "core/log.h"

namespace mixi
{
namespace imgui
{


Application::Application(const char* app_name, int weight, int height) :
    glfw_(app_name, weight, height),
    imgui_(glfw_.glfw_window()),
    context_{
        .glfw_window = glfw_.glfw_window(),
        .io_context = io_context_,
    }
{

}

Application::~Application()
{

}

void Application::on_render_(
    boost::asio::steady_timer* timer,
    BaseComponent* main_window,
    int ms
) {
    timer->expires_after(boost::asio::chrono::milliseconds(ms));

    glfw_.pre_render();
    imgui_.pre_render();
    main_window->render();
    imgui_.post_render();
    glfw_.post_render();

    if (should_close_()) {
        io_context_.stop();
        return;
    }

    timer->async_wait(boost::bind(
        &Application::on_render_, this,
        timer, main_window, ms
    ));
}

Context& Application::context()
{
    return context_;
}

void Application::loop(BaseComponent* main_window, int ms)
{
    boost::asio::steady_timer timer(io_context_);
    timer.async_wait(boost::bind(
        &Application::on_render_, this,
        &timer, main_window, ms
    ));
    io_context_.run();
}

bool Application::should_close_() const
{
    return glfwWindowShouldClose(glfw_.glfw_window());
}


} // namespace gui
} // namespace mixi

