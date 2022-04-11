#pragma once

#include "pch.h"
#include "gui/imgui/popup.h"

namespace mixi
{
namespace gobang
{

class ModalRoomSearch : public imgui::PopupModal
{

public:

    ModalRoomSearch();
    ~ModalRoomSearch();

    void content() override;

private:


};

}
} // namespace mixi
