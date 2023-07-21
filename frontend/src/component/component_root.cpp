#include "component/component_root.h"

#include "core/i18n.h"

namespace mixi {
namespace gobang {

ComponentRoot::ComponentRoot(gui::Context& context) :
    gui::Component<ComponentRoot>(context),
    work_guard_(net_ctx_.get_executor()),
    server_game_room_(net_ctx_),
    is_gaming_(false)
{
    ImGuiIO &io = ImGui::GetIO();

    io.Fonts->AddFontFromFileTTF(
        "resource/font/HarmonyOS_Sans_SC_Regular.ttf", 22.0f,
        nullptr, io.Fonts->GetGlyphRangesChineseFull()
    );

    ImGui::GetStyle().FrameRounding = 6.0f;
    ImGui::GetStyle().PopupRounding = 12.0f;


    std::thread thread([this]() {
        net_ctx_.run();
    });
    thread.detach();

    server_game_room_.on_join_room(
        std::bind(&ComponentRoot::on_join_room, this, std::placeholders::_1)
    );

}

ComponentRoot::~ComponentRoot()
{

}

void ComponentRoot::content()
{
    if (is_gaming_) {
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
}

void ComponentRoot::render_home_window_()
{
    if (window_home_.get() == nullptr) {
        component_room_.reset();
        window_home_.reset(new WindowHome(context_, server_game_room_));
    }
    window_home_->render();
}

void ComponentRoot::on_join_room(ServerGameRoom::JoinRoomState state)
{
    is_gaming_ = (state == ServerGameRoom::JoinRoomState::JOINED);
}

} // namespace gobang
} // namespace mixi