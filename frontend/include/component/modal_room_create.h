#pragma once

#include "pch.h"
#include "mixi/gui/component.h"


namespace mixi
{
namespace gobang
{

class ModalRoomCreate : public gui::PopupModal
{

public:

    ModalRoomCreate(gui::Context& context);
    ~ModalRoomCreate();

    void content() override;
    
};

}
} // namespace mixi
