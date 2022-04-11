#include "component/modal_room_create.h"

namespace mixi
{
namespace gobang
{

ModalRoomCreate::ModalRoomCreate() :
    PopupModal(gettext("Create Room"))
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
