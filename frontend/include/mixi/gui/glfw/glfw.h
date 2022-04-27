#pragma once
#include "pch.h"

namespace mixi
{
namespace glfw
{

class Glfw
{
public:

    Glfw(const char* title, int width = 1280, int height = 720);
    ~Glfw();

    GLFWwindow* glfw_window() const;

    void full_screen(bool is_full_screen);
    bool full_screen();

    void pre_render() const;
    void post_render() const;

private:

    GLFWwindow* glfw_window_;

    bool full_screen_;

    const int default_width_;
    const int default_height_;

    static void Error_Callback_(int error, const char* description);

};

} // namespace glfw
} // namespace mixi