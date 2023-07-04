#pragma once
#include "pch.h"

namespace mixi
{
namespace gui
{

class GlfwWindow
{
public:

    GlfwWindow(const char* title, int width, int height);

    GLFWwindow* glfw_window() const;

    bool should_close() const;

    void full_screen(bool is_full_screen);
    bool full_screen() const;

    void pre_render() const;
    void post_render() const;

private:

    GLFWwindow* glfw_window_;

    bool full_screen_;

    const int default_width_;
    const int default_height_;

};

class Glfw
{
public:

    ~Glfw();
    static Glfw& Instance();

private:

    Glfw();
    Glfw(const Glfw&) = delete;
    Glfw& operator=(const Glfw&) = delete;

    static void Error_Callback_(int error, const char* description);

};



} // namespace glfw
} // namespace mixi