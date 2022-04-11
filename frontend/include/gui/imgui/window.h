#pragma once
#include "pch.h"
#include "gui/imgui/base/component.h"


namespace mixi
{
namespace imgui
{

class Window : public BaseComponent
{
public:

    Window(const std::string &name, ImGuiWindowFlags flags = 0);

    void render() override;

protected:

    const std::string name_;
    ImGuiWindowFlags flags_;

    bool *p_open_;

private:

    Window *component_;

};

} // namespace imgui
} // namespace mixi

