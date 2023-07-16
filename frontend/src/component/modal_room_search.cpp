#include "component/modal_room_search.h"
#include "core/log.h"

namespace mixi
{
namespace gobang
{

ModalRoomSearch::ModalRoomSearch(gui::Context& context, ServerGameRoom& server_game_room) :
    PopupModal(context, gettext("Search Room"), ImGuiWindowFlags_AlwaysAutoResize),
    item_current_idx_(-1),
    search_ip_{0xFF, 0xFF, 0xFF, 0xFF},
    search_port_(52039),
    server_room_search_(server_game_room.io_context),
    server_game_room_(server_game_room),
    hint_(Hint_Search_)
{
    username_[0] = '\0';
    password_[0] = '\0';
}

ModalRoomSearch::~ModalRoomSearch()
{
    // io_context_room_search_.stop();
}

void ModalRoomSearch::content()
{
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", gettext("Server Address"));
    for (int i = 0; i < 4; i++) {
        ImGui::SameLine();
        ImGui::PushID(i);
        ImGui::PushItemWidth(40);
        ImGui::InputScalar("##empty", ImGuiDataType_U8, &search_ip_[i],  NULL, NULL, "%u");
        ImGui::PopItemWidth();
        ImGui::PopID();
    }
    ImGui::SameLine();
    ImGui::Text(":");
    ImGui::SameLine();
    ImGui::PushItemWidth(70);
    ImGui::InputScalar("##empty", ImGuiDataType_U16, &search_port_,  NULL, NULL, "%u");
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button(gettext("Search"))) {
        on_search_();
    }

    ImGui::Text("%s", gettext("Rooms List"));
    
    update_rooms_();
    
    if (ImGui::BeginListBox("##listbox 2",
        ImVec2(600, 5 * ImGui::GetTextLineHeightWithSpacing())
    )) {
        for (int i = 0; i < rooms_.size(); i++) {
            const bool is_selected = (item_current_idx_ == i);
            if (ImGui::Selectable(rooms_[i].room_name.c_str(), is_selected)) {
                item_current_idx_ = i;
            }
            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
            ImGui::SameLine(280);
            ImGui::Text("%s", rooms_[i].endpoint.address().to_string().c_str());
            ImGui::SameLine(450);
            if (rooms_[i].is_playing) {
                ImGui::Text("%s", gettext("Playing"));
            } else {
                ImGui::Text("%s", gettext("Waiting"));
            }
        }
        ImGui::EndListBox();
    }

    ImGui::Separator();

    const ConciseRoom* select_room = nullptr;
    if (item_current_idx_ >= 0 && item_current_idx_ < rooms_.size()) {
        select_room = &rooms_[item_current_idx_];
    }
    ImGui::Text("%s:", gettext("Room Name"));
    if (select_room != nullptr) {
        ImGui::SameLine(200);
        ImGui::Text("%s", select_room->room_name.c_str());
    }

    ImGui::Text("%s:", gettext("Server Address"));
    if (select_room != nullptr) {
        ImGui::SameLine(200);
        ImGui::Text("%s:%d",
            select_room->endpoint.address().to_string().c_str(),
            select_room->endpoint.port()
        );
    }


    ImGui::Text("%s:", gettext("Black Player"));
    if (select_room != nullptr) {
        ImGui::SameLine(200);
        ImGui::Text("%s", select_room->black_player.c_str());
    }

    ImGui::Text("%s:", gettext("White Player"));
    if (select_room != nullptr) {
        ImGui::SameLine(200);
        ImGui::Text("%s", select_room->white_player.c_str());
    }

    ImGui::Text("%s:", gettext("Number of Users"));
    if (select_room != nullptr) {
        ImGui::SameLine(200);
        ImGui::Text("%d / %d",
            select_room->users,
            select_room->max_users
        );
    }

    ImGui::Text("%s:", gettext("State"));
    if (select_room != nullptr) {
        ImGui::SameLine(200);
        if (select_room->is_playing) {
            ImGui::Text("%s", gettext("Playing"));
        } else {
            ImGui::Text("%s", gettext("Waiting"));
        }
    }

    ImGui::NewLine();
    ImGui::Text("%s:", gettext("Your Nickname"));
    ImGui::SameLine(200);
    ImGui::PushItemWidth(180);
    ImGui::InputText("##playername", username_, 64);
    ImGui::PopItemWidth();
    ImGui::SameLine();

    char role = '-';
    ImGui::BeginDisabled(select_room == nullptr);
    if (ImGui::Button(gettext("Join Game"))) {
        role = 'P';
    }
    ImGui::SameLine();
    if (ImGui::Button(gettext("Watch Just"))) {
        role = 'O';
    }
    ImGui::EndDisabled();

    if (role != '-') {
        server_game_room_.join_room(
            rooms_[item_current_idx_].endpoint,
            rooms_[item_current_idx_].room_id,
            username_,
            password_,
            role
        );
    }

    ImGui::Text("%s", hint_);
}

void ModalRoomSearch::update_rooms_()
{
    std::vector<ConciseRoom> new_rooms = server_room_search_.new_rooms();
    rooms_.insert(rooms_.end(), new_rooms.begin(), new_rooms.end());
}

void ModalRoomSearch::on_search_()
{
    rooms_.clear();
    boost::asio::ip::address_v4 address = boost::asio::ip::make_address_v4(search_ip_);
    server_room_search_.search_room(boost::asio::ip::udp::endpoint(address, search_port_));
}

// bool ModalRoomSearch::should_join_room() const
// {
//     return should_join_room_;
// }

// std::pair<ConciseRoom, bool> ModalRoomSearch::info_join_room() const
// {
//     return {rooms_[item_current_idx_], as_a_player_};
// }


// void ModalRoomSearch::join_room_(bool is_player)
// {
//     hint_ = Hint_Joining_;
//     server_game_room_.join_room(
//         rooms_[item_current_idx_].endpoint,
//         rooms_[item_current_idx_].id,
//         player_name_,
//         is_player
//     );
// }

const char* ModalRoomSearch::Hint_Search_  = gettext("Please search rooms and choose one to join");
const char* ModalRoomSearch::Hint_Joining_ = gettext("Joining room, please wait");
const char* ModalRoomSearch::Hint_Failed_  = gettext("Failed to join room");


} // namespace gobang
} // namespace mixi
