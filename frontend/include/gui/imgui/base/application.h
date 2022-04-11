#pragma once
#include "pch.h"

#include "gui/imgui/base/component.h"

namespace mixi
{
namespace imgui
{


class Application
{
public:

    Application(const char* app_name, int weight = 1280, int height = 720);
    ~Application();

    void loop(BaseComponent* main_window) const;

private:

    class Glfw
    {
    public:

        Glfw(const char* title, int weight = 1280, int height = 720);
        ~Glfw();

        GLFWwindow* glfw_window() const;

        void pre_render() const;
        void post_render() const;

    private:

        GLFWwindow* glfw_window_;

        static void Error_Callback_(int error, const char* description);

    };


    class Imgui
    {
    public:

        Imgui(GLFWwindow* glfw_window);
        ~Imgui();

        void pre_render() const;
        void post_render() const;

    };

    Glfw glfw_;
    Imgui imgui_;

    bool should_close_() const;

};

} // namespace imgui
} // namespace mixi

