#include "component/modal_confirm_leave.h"

namespace mixi
{
namespace gobang
{

ModalConfirmLeave::ModalConfirmLeave(gui::Context& context) :
    PopupModal(context, gettext("Exit"), ImGuiWindowFlags_AlwaysAutoResize),
    leave_(false)
{

}

void ModalConfirmLeave::content()
{
    ImGui::Text("%s", gettext("Are you sure to exit the room?"));

    if (ImGui::Button(gettext("Exit"))) {
        leave_ = true;
        ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button(gettext("Cancel"))) {
        ImGui::CloseCurrentPopup();
    }
}

bool ModalConfirmLeave::leave()
{
    return leave_;
}

} // namespace gobang
} // namespace mixi
