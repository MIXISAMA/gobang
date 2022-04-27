#include "component/component_root.h"

#include "core/i18n.h"

namespace mixi {
namespace gobang {

ComponentRoot::ComponentRoot(imgui::Context& context) :
    imgui::BaseComponent(context),
    server_game_room_(context_.io_context),
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
        if (component_room_.get() == nullptr) {
            window_home_.reset();
            component_room_.reset(
                new ComponentRoom(context_, server_game_room_)
            );
        }
        component_room_->render();
        gaming_ = !component_room_->leave_done();
    } else {
        if (window_home_.get() == nullptr) {
            component_room_.reset();
            window_home_.reset(
                new WindowHome(context_, server_game_room_)
            );
        }
        window_home_->render();
        gaming_ = window_home_->join_done();
    }
}

} // namespace gobang
} // namespace mixi