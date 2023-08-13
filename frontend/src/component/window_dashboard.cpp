#include "component/window_dashboard.h"

#include "mixi/image/image.h"

namespace mixi {
namespace gobang {

WindowDashboard::WindowDashboard(
    gui::Context& context,
    const ReadFirstBuffer<game::Room>& room,
    ReadTryQueue<game::User>& user_queue
) :
    gui::Window(context, gettext("Dashboard")),
    room_(room),
    user_queue_(user_queue),
    modal_confirm_leave_(context),
    on_ready_([](){}),
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

void WindowDashboard::on_ready(const std::function<void()>& f)
{
    on_ready_ = f;
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

    bool hovered = false;

    ImGui::Text("%s:", gettext("Black"));
    ImGui::SameLine(100);
    ImGui::Text("%s", room->black_player.c_str());
    hovered |= user_info_tool_tips_helper_(room->black_player);

    ImGui::Text("%s:", gettext("White"));
    ImGui::SameLine(100);
    ImGui::Text("%s", room->white_player.c_str());
    hovered |= user_info_tool_tips_helper_(room->white_player);

    ImGui::Text("%s:", gettext("Onlookers"));
    if (ImGui::BeginListBox(
            "##onlookers",
            ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())
    )) {
        for (const std::string& onlooker : room->onlookers) {
            ImGui::Selectable(onlooker.c_str());
            hovered |= user_info_tool_tips_helper_(onlooker);
        }
        ImGui::EndListBox();
    }

    hovered_ = hovered;

    RtqReader<game::User> user_queue(user_queue_);
    if (user_queue.locked() && !user_queue.empty()) {
        game::User user = user_queue.pop();
        users_[user.username] = user;
    }

    if (role_ == game::SPACE) {
        return;
    }

    float width = ImGui::GetContentRegionAvail().x;
    if (room->is_playering && room->ready_player == role_) {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button(gettext("Ready"), ImVec2(width, 0))) {
        on_ready_();
    }
    if (room->is_playering && room->ready_player == role_) {
        ImGui::EndDisabled();
    }
    if (!room->is_playering) {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button(gettext("Regret"), ImVec2(width, 0))) {
        on_regret_();
    }
    if (ImGui::Button(gettext("Tie"), ImVec2(width, 0))) {
        on_tie_();
    }
    if (ImGui::Button(gettext("Give Up"), ImVec2(width, 0))) {
        on_give_up_();
    }
    if (!room->is_playering) {
        ImGui::EndDisabled();
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

bool WindowDashboard::user_info_tool_tips_helper_(
    const std::string& username
) {
    if (!ImGui::IsItemHovered()) {
        return false;
    }
    if (!hovered_) {
        on_user_info_(username);
    }

    ImGui::BeginTooltip();

    std::map<std::string, game::User>::iterator iter = users_.find(username);
    if (iter != users_.end()) {

        ImGui::Text("%s", iter->second.username.c_str());

        ImGui::BeginTable("User-Info-Table", 2,
            ImGuiTableFlags_BordersV
          | ImGuiTableFlags_BordersOuterH
          | ImGuiTableFlags_RowBg
          | ImGuiTableFlags_ContextMenuInBody
          | ImGuiTableFlags_NoHostExtendX
        );

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("%s", gettext("Number of Wins"));
        ImGui::TableNextColumn();
        ImGui::Text("%u", iter->second.number_of_wins);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("%s", gettext("Number of Ties"));
        ImGui::TableNextColumn();
        ImGui::Text("%u", iter->second.number_of_ties);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("%s", gettext("Number of Losses"));
        ImGui::TableNextColumn();
        ImGui::Text("%u", iter->second.number_of_losses);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("%s", gettext("Number of Matches"));
        ImGui::TableNextColumn();
        ImGui::Text("%u", iter->second.number_of_matches);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("%s", gettext("Game Duration"));
        ImGui::TableNextColumn();
        ImGui::Text("%u Hours", iter->second.game_duration / 3600u);

        ImGui::EndTable();
    }
    else {
        ImGui::Text("...");
    }

    ImGui::EndTooltip();

    return true;
}

} // namespace gobang
} // namespace mixi
