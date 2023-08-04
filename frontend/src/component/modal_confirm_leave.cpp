#include "component/modal_confirm_leave.h"

namespace mixi
{
namespace gobang
{

ModalConfirmLeave::ModalConfirmLeave(gui::Context& context) :
    PopupModal(context, gettext("Exit"), ImGuiWindowFlags_AlwaysAutoResize),
    on_leave_([](){})
{

}

void ModalConfirmLeave::content()
{
    ImGui::Text("%s", gettext("Are you sure to exit the room?"));

    if (ImGui::Button(gettext("Exit"), ImVec2(80, 0))) {
        on_leave_();
        ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button(gettext("Cancel"), ImVec2(80, 0))) {
        ImGui::CloseCurrentPopup();
    }
}

void ModalConfirmLeave::on_leave(const std::function<void()>& f)
{
    on_leave_ = f;
}

} // namespace gobang
} // namespace mixi
