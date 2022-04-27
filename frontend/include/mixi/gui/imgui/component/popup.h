#pragma once
#include "pch.h"
#include "gui/imgui/component/component.h"


namespace mixi
{
namespace imgui
{

class PopupModal : public BaseComponent
{
public:

    PopupModal(
        Context& context,
        const std::string &name,
        ImGuiWindowFlags flags = 0
    );

    void render() override;

    void open();
    void close();
    bool is_opening();

protected:

    virtual void pre_render();

    const std::string name_;
    ImGuiWindowFlags flags_;

private:

    static long long Id_;

    bool should_close_;

};


} // namespace imgui
} // namespace mixi


