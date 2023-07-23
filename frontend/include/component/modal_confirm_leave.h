#pragma once

#include "pch.h"
#include "mixi/gui/component.h"


namespace mixi
{
namespace gobang
{

class ModalConfirmLeave : public gui::PopupModal
{

public:

    ModalConfirmLeave(gui::Context& context);
    ~ModalConfirmLeave() = default;

    bool leave();

    void content() override;

protected:

    bool leave_;

};

}
} // namespace mixi
