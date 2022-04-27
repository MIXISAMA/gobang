#include "component/modal_room_create.h"

namespace mixi
{
namespace gobang
{

ModalRoomCreate::ModalRoomCreate(imgui::Context& context) :
    PopupModal(context, gettext("Create Room"))
{

}

ModalRoomCreate::~ModalRoomCreate()
{

}

void ModalRoomCreate::content()
{
    ImGui::Text(gettext("Create Room"));
}


} // namespace gobang
} // namespace mixi
