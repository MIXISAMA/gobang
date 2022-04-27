#pragma once
#include "pch.h"

#include "gui/glfw/glfw.h"

namespace mixi
{
namespace imgui
{

struct Context {
    glfw::Glfw& glfw;
    boost::asio::io_context& io_context;
};

} // namespace mixi
} // namespace imgui