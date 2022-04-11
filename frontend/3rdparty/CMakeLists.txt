cmake_minimum_required(VERSION 3.12)

# === glad ===

add_library(glad STATIC ./glad/src/gl.c)
target_include_directories(glad PUBLIC ./glad/include)

# === glad ===
# === imgui ===

add_library(imgui STATIC
    ./imgui/imgui.cpp 
    ./imgui/imgui_draw.cpp 
    ./imgui/imgui_tables.cpp 
    ./imgui/imgui_widgets.cpp 
    ./imgui/backends/imgui_impl_glfw.cpp
    ./imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui
    PUBLIC ./imgui/
    PUBLIC ./imgui/backends/
    PUBLIC /usr/local/include/
)

target_link_libraries(imgui glfw3)

if (APPLE)
    target_link_libraries(imgui "-framework Cocoa -framework IOKit")
endif()

# === imgui ===
# # === inih ===

# add_library(inih STATIC
#     ./inih/ini.c
#     ./inih/cpp/INIReader.cpp
# )

# target_include_directories(glad
#     PUBLIC ./inih/
#     PUBLIC ./inih/cpp/
# )

# # === inih ===