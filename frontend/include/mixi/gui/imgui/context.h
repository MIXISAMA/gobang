#pragma once
#include "pch.h"

namespace mixi
{
namespace imgui
{

struct Context {
    GLFWwindow* glfw_window;
    boost::asio::io_context& io_context;
};

} // namespace mixi
} // namespace imgui