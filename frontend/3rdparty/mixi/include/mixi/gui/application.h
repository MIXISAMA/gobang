#pragma once
#include "mixi/pch.h"

#include "mixi/gui/glfw.h"
#include "mixi/gui/imgui.h"
#include "mixi/gui/base_component.h"

namespace mixi
{
namespace gui
{

struct Context
{
    template <typename T>
    friend class Component;

public:

    GlfwWindow& glfw_window_;

    Context(GlfwWindow& glfw_window);

    template <TBaseComponent T>
    T* get_shared(const std::string& shared_name) {
        return static_cast<T*>(components_[shared_name]);
    }

private:

    // boost::asio::io_context& io_context;
    std::map<std::string, BaseComponent*> components_;

};





class Application
{
public:

    Application(const char* app_name, int weight = 1280, int height = 720);
    ~Application();

    Context& context();

    void run(BaseComponent* main_window, int fps_limit = 30);

private:

    GlfwWindow glfw_window_;
    Imgui imgui_;
    boost::asio::io_context io_context_;
    Context app_context_;

    boost::asio::awaitable<void> loop_rendering_(BaseComponent* window, int fps_limit);

};

} // namespace gui
} // namespace mixi

