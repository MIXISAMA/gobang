#include "mixi/gui/glfw.h"

#include "mixi/core/log.h"

namespace mixi
{
namespace gui
{

GlfwWindow::GlfwWindow(const char* title, int width, int height) :
    full_screen_(false),
    default_width_(width),
    default_height_(height)
{
    Glfw::Instance();
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

    // glEnable(GL_DEPTH_TEST);
    // glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
}

GlfwWindow::~GlfwWindow()
{
    glfwDestroyWindow(glfw_window_);
}

GLFWwindow* GlfwWindow::glfw_window() const
{
    return glfw_window_;
}

void GlfwWindow::pre_render() const
{
    glfwMakeContextCurrent(glfw_window_);
    int display_w, display_h;
    glfwGetFramebufferSize(glfw_window_, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GlfwWindow::post_render() const
{
    glfwPollEvents();
    glfwSwapBuffers(glfw_window_);
}

void GlfwWindow::full_screen(bool is_full_screen)
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

bool GlfwWindow::full_screen() const
{
    return full_screen_;
}


bool GlfwWindow::should_close() const
{
    return glfwWindowShouldClose(glfw_window_);
}


Glfw::Glfw()
{
    glfwSetErrorCallback(Error_Callback_);
    if (!glfwInit()) {
        throw std::runtime_error("glfw init error!");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_DEPTH_BITS, 32);

#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#endif

}

Glfw::~Glfw()
{
    glfwTerminate();
}

Glfw& Glfw::Instance()
{
    static Glfw glfw;
    return glfw;
}


void Glfw::Error_Callback_(int error, const char* description)
{
    std::ostringstream oss;
    oss << "Glfw Error Code " << error << ": " << description;
    Log::Error(oss.str());
}

} // namespace glfw
} // namespace mixi
