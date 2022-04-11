#pragma once

#include "pch.h"
#include "gui/imgui/popup.h"


namespace mixi
{
namespace gobang
{

class ModalRoomCreate : public imgui::PopupModal
{

public:

    ModalRoomCreate();
    ~ModalRoomCreate();

    void content() override;
    
};

}
} // namespace mixi
