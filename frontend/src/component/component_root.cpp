#include "component/component_root.h"

#include "core/i18n.h"

namespace mixi {
namespace gobang {

ComponentRoot::ComponentRoot()
{
    ImGuiIO &io = ImGui::GetIO();

    io.Fonts->AddFontFromFileTTF(
        "resource/font/HarmonyOS_Sans_SC_Regular.ttf", 22.0f,
        nullptr, io.Fonts->GetGlyphRangesChineseFull()
    );

    ImGui::GetStyle().FrameRounding = 6.0f;
    ImGui::GetStyle().PopupRounding = 12.0f;

    window_home_ = WindowHome::Ptr(new WindowHome());

}

ComponentRoot::~ComponentRoot()
{

}

void ComponentRoot::content()
{
    window_home_->render();
}

} // namespace gobang
} // namespace mixi