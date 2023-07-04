#include "component/component_root.h"

#include "core/i18n.h"

namespace mixi {
namespace gobang {

ComponentRoot::ComponentRoot(gui::Context& context) :
    gui::Component<ComponentRoot>(context),
    // server_game_room_(context_.io_context),
    gaming_(false)
{
    ImGuiIO &io = ImGui::GetIO();

    io.Fonts->AddFontFromFileTTF(
        "resource/font/HarmonyOS_Sans_SC_Regular.ttf", 22.0f,
        nullptr, io.Fonts->GetGlyphRangesChineseFull()
    );

    ImGui::GetStyle().FrameRounding = 6.0f;
    ImGui::GetStyle().PopupRounding = 12.0f;

}

ComponentRoot::~ComponentRoot()
{

}

void ComponentRoot::content()
{
    if (gaming_) {
        render_game_room_();
    }
    else {
        render_home_window_();
    }
}

void ComponentRoot::render_game_room_()
{
    if (component_room_.get() == nullptr) {
        window_home_.reset();
        component_room_.reset(new ComponentRoom(context_));
    }
    component_room_->render();
    gaming_ = !component_room_->leave_done();
}

void ComponentRoot::render_home_window_()
{
    if (window_home_.get() == nullptr) {
        component_room_.reset();
        window_home_.reset(new WindowHome(context_));
    }
    window_home_->render();
    gaming_ = window_home_->modal_room_search().should_join_room();
    if (gaming_) {
        auto [room, as_a_player] = window_home_->modal_room_search().info_join_room();
    }
}

} // namespace gobang
} // namespace mixi