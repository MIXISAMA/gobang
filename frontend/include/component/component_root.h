#pragma once
#include "pch.h"

#include "gui/imgui/base/component.h"

#include "component/window_home.h"

namespace mixi {
namespace gobang {

class ComponentRoot : public imgui::BaseComponent
{
public:

    ComponentRoot();
    ~ComponentRoot();

    void content() override;

private:

    WindowHome::Ptr window_home_;

};

} // namespace gobang
} // namespace mixi