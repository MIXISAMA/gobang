#pragma once
#include "pch.h"

namespace mixi
{
namespace glfw
{

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

} // namespace glfw
} // namespace mixi