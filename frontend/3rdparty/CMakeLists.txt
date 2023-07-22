cmake_minimum_required(VERSION 3.12)


# === glfw ===

add_subdirectory(glfw)

# === glfw ===
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
    PUBLIC glfw/include/
)

target_link_libraries(imgui glfw)

if (APPLE)
    target_link_libraries(imgui "-framework Cocoa -framework IOKit")
endif()

# === imgui ===
# === assimp ===

add_subdirectory(assimp)

# === assimp ===
# === boost ===

set(BOOST_INCLUDE_LIBRARIES asio thread uuid signals2)
add_subdirectory(boost EXCLUDE_FROM_ALL)
# add_subdirectory(boost/libs/thread EXCLUDE_FROM_ALL)
# add_subdirectory(boost)

# === boost ===
# === mixi ===


file(GLOB_RECURSE MIXI_SOURCE_LIST CONFIGURE_DEPENDS "mixi/src/*.cpp")

add_library(mixi STATIC ${MIXI_SOURCE_LIST})
target_include_directories(
    mixi
    PUBLIC mixi/include/
    PUBLIC glad/include/
    # PUBLIC glfw/include/
    PUBLIC imgui/
    PUBLIC imgui/backends/
    PUBLIC glm/
    PUBLIC assimp/include/
    PUBLIC stb/
)


target_link_directories(mixi
    PUBLIC ${CMAKE_BINARY_DIR}/3rdparty/
    PUBLIC ${ASSIMP_ARCHIVE_OUTPUT_DIRECTORY}
    PUBLIC ${GLFW_BINARY_DIR}/src
)

target_link_libraries(mixi
    assimp
    glad
    glfw
    imgui
    Boost::asio
    Boost::thread
    ${Intl_LIBRARIES}
    # Boost
    # ${Boost_SYSTEM_LIBRARY_RELEASE}
)

# === mixi ===
