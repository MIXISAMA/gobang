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

    void on_leave(const std::function<void()>& f);

protected:

    std::function<void()> on_leave_;

    void content() override;

};

}
} // namespace mixi
