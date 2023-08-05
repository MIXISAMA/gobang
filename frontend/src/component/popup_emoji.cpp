#include "component/popup_emoji.h"
namespace mixi
{
namespace gobang
{

PopupEmoji::PopupEmoji(gui::Context& context) :
    Popup(context, "popup_emoji"),
    on_selected_([](const char*){}),
    on_backspace_([](){}),
    selectable_flags_(ImGuiSelectableFlags_None)
{

}

void PopupEmoji::content()
{
    ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (x > 0) {
                ImGui::SameLine();
            }
            if (ImGui::Selectable(AllEmoji_[y * 8 + x], false, selectable_flags_, ImVec2(30, 30))) {
                on_selected_(AllEmoji_[y * 8 + x]);
            }
        }
    }
    ImGui::PopStyleVar();

    if (ImGui::IsKeyDown(ImGuiKey_Backspace)) {
        if (!keys_down_) {
            on_backspace_();
            keys_down_ = true;
        }
    }
    else {
        keys_down_ = false;
    }
}

void PopupEmoji::on_selected(const std::function<void(const char*)>& f)
{
    on_selected_ = f;
}

void PopupEmoji::on_backspace(const std::function<void(void)>& f)
{
    on_backspace_ = f;
}

void PopupEmoji::selectable_flags(ImGuiSelectableFlags flags)
{
    selectable_flags_ = flags;
}

const char* PopupEmoji::AllEmoji_[64] = {
    "😃", "🤣", "😌", "😘", "😎", "🤔", "🫣", "😏",
    "😴", "🙄", "😫", "😮‍💨", "😳", "😅", "🌚", "🙃",
    "🐶", "🐱", "🐷", "🐼", "🐻", "🐵", "🐴", "🐮",
    "🍎", "🍊", "🍐", "🍌", "🍉", "🍒", "🍍", "🥭",
    "🍦", "🧋", "🍺", "🍣", "🍩", "🍔", "🍕", "🍮",
    "🏀", "⚽", "⚾", "🎱", "🏈", "🏸", "🏓", "🀄",
    "👀", "🤙", "👍", "👎", "🤦‍♀️", "🤷‍♀️", "💩", "🤡",
    "💭", "💡", "🎈", "💕", "💦", "🎉", "💰", "👽",
};

} // namespace gobang
} // namespace mixi
