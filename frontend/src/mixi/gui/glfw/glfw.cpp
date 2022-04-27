#include "gui/glfw/glfw.h"

#include "core/log.h"

namespace mixi
{
namespace glfw
{

Glfw::Glfw(const char* title, int width, int height) :
    full_screen_(false),
    default_width_(width),
    default_height_(height)
{
    glfwSetErrorCallback(Error_Callback_);
    if (!glfwInit()) {
        throw std::runtime_error("glfw init error!");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only

#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#endif

    glfw_window_ = (GLFWwindow*)glfwCreateWindow(width, height, title, NULL, NULL);
    if (glfw_window_ == nullptr) {
        glfwTerminate();
        throw std::runtime_error("create main window error!");
    }
    glfwMakeContextCurrent(glfw_window_);
    if (!gladLoadGL(glfwGetProcAddress)) {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD!");
    }

    glfwSetWindowSizeLimits(glfw_window_, 600, 400, GLFW_DONT_CARE, GLFW_DONT_CARE);

    glfwSwapInterval(1); // Enable vsync

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
}

Glfw::~Glfw()
{
    glfwTerminate();
}

GLFWwindow* Glfw::glfw_window() const
{
    return glfw_window_;
}

void Glfw::pre_render() const
{
    int display_w, display_h;
    glfwGetFramebufferSize(glfw_window_, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Glfw::post_render() const
{
    glfwPollEvents();
    glfwSwapBuffers(glfw_window_);
}

void Glfw::full_screen(bool is_full_screen)
{
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwSetWindowMonitor(
        glfw_window_,
        is_full_screen ? monitor : NULL,
        0, 0,
        is_full_screen ? mode->width: default_width_,
        is_full_screen ? mode->height: default_height_,
        GLFW_DONT_CARE
    );
    full_screen_ = is_full_screen;
}

bool Glfw::full_screen()
{
    return full_screen_;
}

void Glfw::Error_Callback_(int error, const char* description)
{
    std::ostringstream oss;
    oss << "Glfw Error Code " << error << ": " << description;
    Log::Error(oss.str());
}

} // namespace glfw
} // namespace mixi
