#include "component/modal_room_search.h"

namespace mixi
{
namespace gobang
{

ModalRoomSearch::ModalRoomSearch() :
    PopupModal(gettext("Search Room"))
{

}

ModalRoomSearch::~ModalRoomSearch()
{

}

void ModalRoomSearch::content()
{
    ImGui::Text(gettext("Rooms List"));
    ImGui::SameLine();
    ImGui::Button(gettext("Search"));
    ImGui::Separator();

    const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
    static int item_current_idx = 0;

    if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
    {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
        {
            const bool is_selected = (item_current_idx == n);
            if (ImGui::Selectable(items[n], is_selected))
                item_current_idx = n;

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }
}


} // namespace gobang
} // namespace mixi
