#pragma once
#include "mixi/pch.h"

namespace mixi
{
namespace gui
{

class Imgui
{
public:

    Imgui(GLFWwindow* glfw_window);
    ~Imgui();

    void pre_render() const;
    void post_render() const;

protected:

    ImGuiContext* context_;

};

} // namespace gui
} // namespace mixi
