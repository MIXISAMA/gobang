#pragma once
#include "mixi/pch.h"

#include "mixi/gui/base_component.h"
#include "mixi/gui/application.h"

namespace mixi
{
namespace gui
{

template <typename T>
class Component : public BaseComponent
{
public:

    Component(Context& context) : context_(context) {}

    void share(const std::string& shared_name)
    {
        if (shared_name.empty() && shared_name_.empty()) {
            return;
        }
        context_.components_.erase(shared_name_);
        shared_name_ = shared_name;
        if (!shared_name.empty()) {
            context_.components_[shared_name] = this;
        }
    }

    static T* Get_Shared(const Context& context, const std::string& shared_name)
    {
        return static_cast<T*>(context.components_.find(shared_name)->second);
    }

protected:

    Context& context_;
    std::string shared_name_;

};

class NameAble
{
public:

    NameAble(const std::string& n = "");

    void name(const std::string& n);
    const std::string& name();

protected:

    std::string name_;

};



class FlagsAble
{
public:

    FlagsAble(ImGuiWindowFlags f = 0);

    void flags(ImGuiWindowFlags f);
    ImGuiWindowFlags flags();

protected:

    ImGuiWindowFlags flags_;

};

class Popup:
    public Component<Popup>,
    public NameAble
{
public:

    Popup(
        Context& context,
        const std::string name
    );

    void render() override;

    void open();
};

class PopupModal :
    public Component<PopupModal>,
    public NameAble,
    public FlagsAble
{
public:

    PopupModal(
        Context& context,
        const std::string name,
        ImGuiWindowFlags flag = ImGuiWindowFlags_AlwaysAutoResize
    );

    void render() override;

    void open();
    void close();
    bool is_opening();

protected:

    virtual void pre_render();

private:

    bool should_close_;

};


class Window :
    public Component<Window>,
    public NameAble,
    public FlagsAble
{
public:

    Window(Context& context, const std::string name);

    void render() override;

protected:

    bool *p_open_;

private:

    Window *component_;

};



} // namespace gui
} // namespace mixi
