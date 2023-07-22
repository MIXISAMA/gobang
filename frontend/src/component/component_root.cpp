#include "component/component_root.h"

#include "core/i18n.h"

namespace mixi {
namespace gobang {

ComponentRoot::ComponentRoot(gui::Context& context) :
    gui::Component<ComponentRoot>(context),
    net_ctx_(),
    net_work_guard_(net_ctx_.get_executor()),
    net_thread_([this]() {net_ctx_.run();}),
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

    server_game_room_.on_join_room(
        std::bind(&ComponentRoot::on_join_room, this, std::placeholders::_1)
    );

}

ComponentRoot::~ComponentRoot()
{
    net_work_guard_.reset();
    net_ctx_.stop();
    net_thread_.join();
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