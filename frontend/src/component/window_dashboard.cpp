#include "component/window_dashboard.h"

#include "mixi/image/image.h"

namespace mixi {
namespace gobang {

WindowDashboard::WindowDashboard(
    gui::Context& context,
    const game::Room& room
) :
    gui::Window(context, gettext("Dashboard")),
    room_(room),
    modal_confirm_leave_(context)
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
    // bool full_screen = context_.glfw.full_screen();
    // if (ImGui::ImageButton(
    //         full_screen ?
    //         (void*)(intptr_t)tex_window_.id() :
    //         (void*)(intptr_t)tex_full_screen_.id(),
    //         ImVec2(32, 32)
    // )) {
    //     context_.glfw.full_screen(!full_screen);
    // }
    // ImGui::SameLine();
    // if (ImGui::ImageButton(
    //         (void*)(intptr_t)tex_exit_.id(),
    //         ImVec2(32, 32)
    // )) {
    //     modal_confirm_leave_.open();
    // }

    // ImGui::Separator();

    // ImGui::Text("%s", room_.name().c_str());
    // ImGui::NewLine();

    // const game::User::Ptr& bp = room_.player(game::Chess::Color::BLACK);
    // const game::User::Ptr& wp = room_.player(game::Chess::Color::WHITE);

    // ImGui::Text("%s:", gettext("Black"));
    // if (bp.get() != nullptr) {
    //     ImGui::SameLine(100);
    //     ImGui::Text("%s", bp->name.c_str());
    // }

    // ImGui::Text("%s:", gettext("White"));
    // if (wp.get() != nullptr) {
    //     ImGui::SameLine(100);
    //     ImGui::Text("%s", wp->name.c_str());
    // }

    // ImGui::Text("%s:", gettext("Onlookers"));
    // if (ImGui::BeginListBox(
    //         "##onlookers",
    //         ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())
    // )) {
    //     for (const game::User::Ptr& user : room_.users()) {
    //         if (user == bp || user == wp) {
    //             continue;
    //         }
    //         ImGui::Selectable(user->name.c_str());
    //     }
    //     ImGui::EndListBox();
    // }

    modal_confirm_leave_.render();
}

void WindowDashboard::load_texture_by_image_(
    gl::Texture2D& tex,
    const std::filesystem::path& filepath
) {
    img::Image icon(filepath);
    icon.inverted_color();
    tex.update_image(
        icon.width(),
        icon.height(),
        gl::Texture2D::Format::RGBA,
        icon.data()
    );
}

bool WindowDashboard::leave()
{
    return modal_confirm_leave_.leave();
}

} // namespace gobang
} // namespace mixi
