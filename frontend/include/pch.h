#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <shared_mutex>
#include <sstream>
#include <vector>

#include "glad/gl.h"
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <libintl.h>

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#define VERSION "0.1"

namespace mixi
{
    namespace
    {
        using std::byte;
        using std::int32_t;
        using std::uint32_t;
        namespace fs = std::filesystem;
        // namespace bp = boost::process;
    }
}
