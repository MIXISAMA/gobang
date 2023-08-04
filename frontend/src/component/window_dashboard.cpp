#include "component/window_dashboard.h"

#include "mixi/image/image.h"

namespace mixi {
namespace gobang {

WindowDashboard::WindowDashboard(
    gui::Context& context,
    const ReadFirstBuffer<game::Room>& room
) :
    gui::Window(context, gettext("Dashboard")),
    room_(room),
    modal_confirm_leave_(context),
    on_regret_([](){}),
    on_tie_([](){}),
    on_give_up_([](){}),
    on_user_info_([](const std::string&){})
{
    load_texture_by_image_(tex_exit_, "resource/image/exit.png");
    load_texture_by_image_(tex_window_, "resource/image/exit-full-screen.png");
    load_texture_by_image_(tex_full_screen_, "resource/image/full-screen.png");
}

void WindowDashboard::role(std::byte r)
{
    role_ = r;
}

void WindowDashboard::on_leave(const std::function<void()>& f)
{
    modal_confirm_leave_.on_leave(f);
}

void WindowDashboard::on_regret(const std::function<void()>& f)
{
    on_regret_ = f;
}

void WindowDashboard::on_tie(const std::function<void()>& f)
{
    on_tie_ = f;
}

void WindowDashboard::on_give_up(const std::function<void()>& f)
{
    on_give_up_ = f;
}

void WindowDashboard::on_user_info(const std::function<void(const std::string&)>& f)
{
    on_user_info_ = f;
}

void WindowDashboard::content()
{
    bool full_screen = context_.glfw_window_.full_screen();
    if (ImGui::ImageButton(
            full_screen ?
            (void*)(intptr_t)tex_window_.id() :
            (void*)(intptr_t)tex_full_screen_.id(),
            ImVec2(32, 32)
    )) {
        context_.glfw_window_.full_screen(!full_screen);
    }
    ImGui::SameLine();
    if (ImGui::ImageButton(
            (void*)(intptr_t)tex_exit_.id(),
            ImVec2(32, 32)
    )) {
        modal_confirm_leave_.open();
    }
    modal_confirm_leave_.render();

    RfbReader<game::Room> room(room_);

    ImGui::Separator();

    ImGui::Text("%s", room->name.c_str());
    ImGui::NewLine();

    ImGui::Text("%s:", gettext("Black"));
    ImGui::SameLine(100);
    ImGui::Text("%s", room->black_player.c_str());

    ImGui::Text("%s:", gettext("White"));
    ImGui::SameLine(100);
    ImGui::Text("%s", room->white_player.c_str());

    ImGui::Text("%s:", gettext("Onlookers"));
    if (ImGui::BeginListBox(
            "##onlookers",
            ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())
    )) {
        for (const std::string& onlooker : room->onlookers) {
            ImGui::Selectable(onlooker.c_str());
        }
        ImGui::EndListBox();
    }

    if (role_ == game::SPACE) {
        return;
    }

    if (ImGui::Button(gettext("Regret"), ImVec2(84, 0))) {
        on_regret_();
    }
    ImGui::SameLine();
    if (ImGui::Button(gettext("Tie"), ImVec2(84, 0))) {
        on_tie_();
    }
    ImGui::SameLine();
    if (ImGui::Button(gettext("Give Up"), ImVec2(84, 0))) {
        on_give_up_();
    }
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

} // namespace gobang
} // namespace mixi
