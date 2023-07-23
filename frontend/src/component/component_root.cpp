#include "component/component_root.h"

#include "mixi/core/i18n.h"
#include "mixi/engine/opengl/textures_manager.h"
namespace mixi {
namespace gobang {

ComponentRoot::ComponentRoot(gui::Context& context) :
    gui::Component<ComponentRoot>(context),
    net_ctx_(),
    net_work_guard_(net_ctx_.get_executor()),
    net_thread_([this]() {net_ctx_.run();}),
    server_game_room_(net_ctx_),
    joined_game_(false)
{
    ImGuiIO &io = ImGui::GetIO();

    io.Fonts->AddFontFromFileTTF(
        "resource/font/HarmonyOS_Sans_SC_Regular.ttf", 22.0f,
        nullptr, io.Fonts->GetGlyphRangesChineseFull()
    );

    ImGui::GetStyle().FrameRounding = 6.0f;
    ImGui::GetStyle().PopupRounding = 12.0f;    

    join_room_connection_ = server_game_room_.connect_join_room(
        std::bind(&ComponentRoot::on_join_room_, this, std::placeholders::_1)
    );
}

ComponentRoot::~ComponentRoot()
{
    join_room_connection_.disconnect();
    net_work_guard_.reset();
    net_ctx_.stop();
    net_thread_.join();
    gl::eng::TexturesManager::Clear();
}

void ComponentRoot::content()
{
    if (joined_game_) {
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

void ComponentRoot::on_join_room_(ServerGameRoom::JoinRoomState state)
{
    joined_game_ = (state == ServerGameRoom::JoinRoomState::JOINED);
}

} // namespace gobang
} // namespace mixi