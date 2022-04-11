#pragma once
#include "pch.h"
#include "gui/imgui/base/component.h"


namespace mixi
{
namespace imgui
{

class PopupModal : public BaseComponent
{
public:

    PopupModal(const std::string &name, ImGuiWindowFlags flags = 0);

    void render() override;

    void open();

protected:

    virtual void pre_render();

    const std::string name_;
    ImGuiWindowFlags flags_;

private:

    static long long Id_;

};


} // namespace imgui
} // namespace mixi


