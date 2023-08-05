#include "component/modal_confirm.h"

namespace mixi
{
namespace gobang
{

/* Leave */
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

/* Regret */
ModalConfirmRegret::ModalConfirmRegret(gui::Context& context) :
    PopupModal(context, gettext("Exit"), ImGuiWindowFlags_AlwaysAutoResize),
    on_agree_regret_([](bool){})
{

}

void ModalConfirmRegret::content()
{
    ImGui::Text("%s", gettext("Opponent hopes to regret."));

    if (ImGui::Button(gettext("Agree"), ImVec2(80, 0))) {
        on_agree_regret_(true);
        ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button(gettext("Disagree"), ImVec2(80, 0))) {
        on_agree_regret_(false);
        ImGui::CloseCurrentPopup();
    }
}

void ModalConfirmRegret::on_agree_regret(const std::function<void(bool)>& f)
{
    on_agree_regret_ = f;
}

/* Tie */
ModalConfirmTie::ModalConfirmTie(gui::Context& context) :
    PopupModal(context, gettext("Exit"), ImGuiWindowFlags_AlwaysAutoResize),
    on_agree_tie_([](bool){})
{

}

void ModalConfirmTie::content()
{
    ImGui::Text("%s", gettext("Opponent hopes to tie."));

    if (ImGui::Button(gettext("Agree"), ImVec2(80, 0))) {
        on_agree_tie_(true);
        ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button(gettext("Disagree"), ImVec2(80, 0))) {
        on_agree_tie_(false);
        ImGui::CloseCurrentPopup();
    }
}

void ModalConfirmTie::on_agree_tie(const std::function<void(bool)>& f)
{
    on_agree_tie_ = f;
}

} // namespace gobang
} // namespace mixi
