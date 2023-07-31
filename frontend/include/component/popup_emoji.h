#pragma once
#include "pch.h"
#include "mixi/gui/component.h"

namespace mixi
{
namespace gobang
{

class PopupEmoji : public gui::Popup
{
public:

    PopupEmoji(gui::Context& context);

    void content() override;
    void on_selected(const std::function<void(const char*)>&);
    void on_backspace(const std::function<void(void)>&);
    void selectable_flags(ImGuiSelectableFlags flags);

private:

    std::function<void(const char*)> on_selected_;
    std::function<void(void)> on_backspace_;
    ImGuiSelectableFlags selectable_flags_;
    bool keys_down_;

    static const char* AllEmoji_[64];

};
    
} // namespace gobang
} // namespace mixi
