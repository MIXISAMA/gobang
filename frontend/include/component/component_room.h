#pragma once
#include "pch.h"
#include "gui/imgui/base/component.h"

#include "component/window_dashboard.h"
#include "component/window_game.h"
#include "component/window_chat.h"

namespace mixi
{
namespace gobang
{

class ComponentRoom : public imgui::BaseComponent
{
public:

    ComponentRoom();
    ~ComponentRoom();

    void content() override;
};
    
} // namespace gobang
} // namespace mixi
