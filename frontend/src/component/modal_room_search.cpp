#include "component/modal_room_search.h"
#include "mixi/core/log.h"

namespace mixi
{
namespace gobang
{

ModalRoomSearch::ModalRoomSearch(gui::Context& context, ServerGameRoom& server_game_room) :
    PopupModal(context, gettext("Search Room"), ImGuiWindowFlags_AlwaysAutoResize),
    search_ip_{0xFF, 0xFF, 0xFF, 0xFF},
    search_port_(52039),
    server_room_search_(server_game_room.io_context),
    server_game_room_(server_game_room),
    hint_(Hint_Search_)
{
    username_[0] = '\0';
    password_[0] = '\0';

    join_room_connection_ = server_game_room.connect_join_room(std::bind(
        &ModalRoomSearch::on_join_room_, this, std::placeholders::_1)
    );
}

ModalRoomSearch::~ModalRoomSearch()
{
    join_room_connection_.disconnect();
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
    
    if (ImGui::BeginListBox(
            "##listbox 2",
            ImVec2(600, 5 * ImGui::GetTextLineHeightWithSpacing())
    )) {
        int i = 0;
        for (const std::shared_ptr<ConciseRoom> room : rooms_) {
            if (ImGui::Selectable(room->room_name.c_str(), room == selected_room_)) {
                selected_room_ = room;
            }
            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (room == selected_room_) {
                ImGui::SetItemDefaultFocus();
            }
            ImGui::SameLine(280);
            ImGui::Text("%s", room->endpoint.address().to_string().c_str());
            ImGui::SameLine(450);
            if (room->is_playing) {
                ImGui::Text("%s", gettext("Playing"));
            } else {
                ImGui::Text("%s", gettext("Waiting"));
            }
            i++;
        }
        ImGui::EndListBox();
    }

    ImGui::Separator();

    ImGui::Text("%s:", gettext("Room Name"));
    if (selected_room_.get() != nullptr) {
        ImGui::SameLine(200);
        ImGui::Text("%s", selected_room_->room_name.c_str());
    }

    ImGui::Text("%s:", gettext("Server Address"));
    if (selected_room_.get() != nullptr) {
        ImGui::SameLine(200);
        ImGui::Text("%s:%d",
            selected_room_->endpoint.address().to_string().c_str(),
            selected_room_->endpoint.port()
        );
    }


    ImGui::Text("%s:", gettext("Black Player"));
    if (selected_room_.get() != nullptr) {
        ImGui::SameLine(200);
        ImGui::Text("%s", selected_room_->black_player.c_str());
    }

    ImGui::Text("%s:", gettext("White Player"));
    if (selected_room_.get() != nullptr) {
        ImGui::SameLine(200);
        ImGui::Text("%s", selected_room_->white_player.c_str());
    }

    ImGui::Text("%s:", gettext("Number of Users"));
    if (selected_room_.get() != nullptr) {
        ImGui::SameLine(200);
        ImGui::Text("%d / %d",
            selected_room_->users,
            selected_room_->max_users
        );
    }

    ImGui::Text("%s:", gettext("State"));
    if (selected_room_.get() != nullptr) {
        ImGui::SameLine(200);
        if (selected_room_->is_playing) {
            ImGui::Text("%s", gettext("Playing"));
        } else {
            ImGui::Text("%s", gettext("Waiting"));
        }
    }

    ImGui::NewLine();
    ImGui::Text("%s:", gettext("Your Username"));
    ImGui::SameLine(200);
    ImGui::PushItemWidth(250);
    ImGui::InputText("##username", username_, sizeof(username_));
    ImGui::PopItemWidth();
    ImGui::SameLine(470);

    char role = '-';
    ImGui::BeginDisabled(selected_room_.get() == nullptr);
    if (ImGui::Button(gettext("Join Game"), ImVec2(120, 0))) {
        role = 'P';
    }
    ImGui::EndDisabled();

    ImGui::Text("%s:", gettext("Your Password"));
    ImGui::SameLine(200);
    ImGui::PushItemWidth(250);
    ImGui::InputText("##password", password_, sizeof(username_));
    ImGui::PopItemWidth();
    ImGui::SameLine(470);

    ImGui::BeginDisabled(selected_room_.get() == nullptr);
    if (ImGui::Button(gettext("Watch Just"), ImVec2(120, 0))) {
        role = 'O';
    }
    ImGui::EndDisabled();

    if (role != '-') {
        server_game_room_.join_room(
            selected_room_->endpoint,
            selected_room_->room_id,
            username_,
            password_,
            role
        );
    }

    ImGui::Text("%s", hint_.load());
}

void ModalRoomSearch::update_rooms_()
{
    RtqReader<ConciseRoom> room(server_room_search_.rooms());
    if (room.locked() && !room.empty()) {
        rooms_.insert(std::make_shared<ConciseRoom>(room.pop()));
    }
}

void ModalRoomSearch::on_search_()
{
    rooms_.clear();
    boost::asio::ip::address_v4 address = boost::asio::ip::make_address_v4(search_ip_);
    server_room_search_.search_room(boost::asio::ip::udp::endpoint(address, search_port_));
}

void ModalRoomSearch::on_join_room_(ServerGameRoom::JoinRoomState state)
{
    switch (state) {
    case ServerGameRoom::JoinRoomState::JOINING:
        hint_ = Hint_Joining_;
        break;
    case ServerGameRoom::JoinRoomState::FAILED:
        hint_ = Hint_Failed_;
        break;
    default:
        hint_ = Hint_Search_;
        break;
    }
}

const char* ModalRoomSearch::Hint_Search_  = gettext("Please search rooms and choose one to join");
const char* ModalRoomSearch::Hint_Joining_ = gettext("Joining room, please wait");
const char* ModalRoomSearch::Hint_Failed_  = gettext("Failed to join room. Maybe duplicate username or wrong password.");


} // namespace gobang
} // namespace mixi
