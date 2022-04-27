#pragma once
#include "pch.h"

namespace mixi
{
namespace imgui
{

class Imgui
{
public:

    Imgui(GLFWwindow* glfw_window);
    ~Imgui();

    void pre_render() const;
    void post_render() const;

};

} // namespace imgui
} // namespace mixi
