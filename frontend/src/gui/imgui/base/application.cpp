#include "gui/imgui/base/application.h"

#include "core/log.h"
#include "core/io_context.h"

namespace mixi
{
namespace imgui
{

Application::Glfw::Glfw(const char* title, int weight, int height)
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

    glfw_window_ = (GLFWwindow*)glfwCreateWindow(weight, height, title, NULL, NULL);
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

Application::Glfw::~Glfw()
{
    glfwTerminate();
}

GLFWwindow* Application::Glfw::glfw_window() const
{
    return glfw_window_;
}

void Application::Glfw::pre_render() const
{
    int display_w, display_h;
    glfwGetFramebufferSize(glfw_window_, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Application::Glfw::post_render() const
{
    glfwPollEvents();
    glfwSwapBuffers(glfw_window_);
}

void Application::Glfw::Error_Callback_(int error, const char* description)
{
    std::ostringstream oss;
    oss << "Glfw Error Code " << error << ": " << description;
    Log::Error(oss.str());
}

Application::Imgui::Imgui(GLFWwindow* glfw_window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigWindowsMoveFromTitleBarOnly=true;
    ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
#if defined(__APPLE__)
    // GL 3.3 + GLSL 150
    const char* glsl_version = "#version 150";
#else
    // GL 3.3 + GLSL 130
    const char* glsl_version = "#version 130";
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);
}

Application::Imgui::~Imgui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Application::Imgui::pre_render() const
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("InvisibleWindow", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    ImGuiID dock_space_id = ImGui::GetID("InvisibleWindowDockSpace");

    ImGui::DockSpace(dock_space_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();
}

void Application::Imgui::post_render() const
{
    ImGui::Render();
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

Application::Application(const char* app_name, int weight, int height) :
    glfw_(app_name, weight, height),
    imgui_(glfw_.glfw_window())
{
    CreateIoContext();
}

Application::~Application()
{
    DestroyIoContext();
}

void Application::on_render_(
    boost::asio::steady_timer* timer,
    BaseComponent* main_window,
    int ms
) const
{
    timer->expires_after(boost::asio::chrono::milliseconds(ms));

    glfw_.pre_render();
    imgui_.pre_render();
    main_window->render();
    imgui_.post_render();
    glfw_.post_render();

    if (should_close_()) {
        Io_Context->stop();
        return;
    }

    timer->async_wait(boost::bind(
        &Application::on_render_, this,
        timer, main_window, ms
    ));
}

void Application::loop(BaseComponent* main_window, int ms)
{
    boost::asio::steady_timer timer(*Io_Context);
    timer.async_wait(boost::bind(
        &Application::on_render_, this,
        &timer, main_window, ms
    ));
    Io_Context->run();
}

bool Application::should_close_() const
{
    return glfwWindowShouldClose(glfw_.glfw_window());
}


} // namespace gui
} // namespace mixi

