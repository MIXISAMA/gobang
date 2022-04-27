#pragma once

#include "pch.h"
#include "gui/imgui/component/popup.h"


namespace mixi
{
namespace gobang
{

class ModalRoomCreate : public imgui::PopupModal
{

public:

    ModalRoomCreate(imgui::Context& context);
    ~ModalRoomCreate();

    void content() override;
    
};

}
} // namespace mixi
