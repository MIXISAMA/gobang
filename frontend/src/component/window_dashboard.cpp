#include "component/window_dashboard.h"

#include "image/image.h"

namespace mixi {
namespace gobang {

WindowDashboard::WindowDashboard(
    imgui::Context& context,
    const game::Room& room
) :
    imgui::Window(context, gettext("Dashboard")),
    room_(room)
{
    load_texture_by_image_(tex_exit_, "resource/image/exit.png");
    load_texture_by_image_(tex_window_, "resource/image/exit-full-screen.png");
    load_texture_by_image_(tex_full_screen_, "resource/image/full-screen.png");
}

WindowDashboard::~WindowDashboard()
{

}

void WindowDashboard::content()
{
    if (ImGui::ImageButton(
            (void*)(intptr_t)tex_full_screen_.id(),
            ImVec2(32, 32)
    )) {
        
    }
    ImGui::SameLine();
    if (ImGui::ImageButton(
            (void*)(intptr_t)tex_exit_.id(),
            ImVec2(32, 32)
    )) {
        
    }
}

void WindowDashboard::load_texture_by_image_(
    gl::Texture2D& tex,
    const std::filesystem::path& filepath
) {
    {
        gl::Bind bind(tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    }

    img::Image icon(filepath);
    icon.inverted_color();
    tex.update_image(
        icon.width(),
        icon.height(),
        gl::Texture2D::Format::RGBA,
        icon.data()
    );
}

} // namespace gobang
} // namespace mixi
