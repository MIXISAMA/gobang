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

class ModalConfirmRegret : public gui::PopupModal
{

public:

    ModalConfirmRegret(gui::Context& context);
    ~ModalConfirmRegret() = default;

    void on_agree_regret(const std::function<void(bool)>& f);

protected:

    std::function<void(bool)> on_agree_regret_;

    void content() override;

};

class ModalConfirmTie : public gui::PopupModal
{

public:

    ModalConfirmTie(gui::Context& context);
    ~ModalConfirmTie() = default;

    void on_agree_tie(const std::function<void(bool)>& f);

protected:

    std::function<void(bool)> on_agree_tie_;

    void content() override;

};

}
} // namespace mixi
