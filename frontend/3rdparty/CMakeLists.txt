cmake_minimum_required(VERSION 3.12)


# === glfw ===

add_subdirectory(glfw)

# === glfw ===
# === glad ===

add_library(glad STATIC ./glad/src/gl.c)
target_include_directories(glad PUBLIC ./glad/include)

# === glad ===
# === freetype ===

set(FT_DISABLE_HARFBUZZ ON)
add_subdirectory(freetype)

# === freetype ===
# === imgui ===

add_library(imgui STATIC
    ./imgui/imgui.cpp 
    ./imgui/imgui_draw.cpp 
    ./imgui/imgui_tables.cpp 
    ./imgui/imgui_widgets.cpp 
    ./imgui/backends/imgui_impl_glfw.cpp
    ./imgui/backends/imgui_impl_opengl3.cpp
    ./imgui/misc/freetype/imgui_freetype.cpp
)

target_compile_definitions(imgui
    PUBLIC IMGUI_ENABLE_FREETYPE
    PUBLIC IMGUI_USE_WCHAR32
)

target_include_directories(imgui
    PUBLIC ./imgui/
    PUBLIC ./imgui/backends/
    PUBLIC ./imgui/misc/freetype/
    PUBLIC ./glfw/include/
    PUBLIC ./freetype/include
)

target_link_libraries(imgui
    glfw
    freetype
)

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
# === intl ===

if (APPLE)
  set(CMAKE_FIND_FRAMEWORK NEVER)
endif()

find_package(Gettext REQUIRED)
find_package(Intl REQUIRED)

# === intl ===
# === mixi ===


file(GLOB_RECURSE MIXI_SOURCE_LIST CONFIGURE_DEPENDS "mixi/src/*.cpp")

add_library(mixi STATIC ${MIXI_SOURCE_LIST})
target_include_directories(
    mixi
    PUBLIC mixi/include/
    PUBLIC glad/include/
    PUBLIC imgui/
    PUBLIC imgui/backends/
    PUBLIC glm/
    PUBLIC assimp/include/
    PUBLIC stb/
    PUBLIC ${Intl_INCLUDE_DIR}
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
)

# === mixi ===
