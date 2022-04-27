#pragma once

#include "pch.h"
#include "gui/imgui/component/popup.h"


namespace mixi
{
namespace gobang
{

class ModalConfirmLeave : public imgui::PopupModal
{

public:

    ModalConfirmLeave(imgui::Context& context);
    ~ModalConfirmLeave() = default;

    bool leave();

    void content() override;

protected:

    bool leave_;

};

}
} // namespace mixi
