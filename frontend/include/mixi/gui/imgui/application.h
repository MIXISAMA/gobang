#pragma once
#include "pch.h"

#include "gui/glfw/glfw.h"
#include "gui/imgui/context.h"
#include "gui/imgui/imgui.h"
#include "gui/imgui/component/component.h"

namespace mixi
{
namespace imgui
{

class Application
{
public:

    Application(const char* app_name, int weight = 1280, int height = 720);
    ~Application();

    Context& context();

    void loop(BaseComponent* main_window, int ms = 1000 / 30);

private:

    glfw::Glfw glfw_;
    Imgui imgui_;
    boost::asio::io_context io_context_;
    Context context_;

    bool should_close_() const;

    void on_render_(
        boost::asio::steady_timer* timer,
        BaseComponent* main_window,
        int ms
    );

};

} // namespace imgui
} // namespace mixi

