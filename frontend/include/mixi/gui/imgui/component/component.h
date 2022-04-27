#pragma once
#include "pch.h"

#include "gui/imgui/context.h"

namespace mixi
{
namespace imgui
{

class BaseComponent
{
public:

    using Ptr = std::shared_ptr<BaseComponent>;

    BaseComponent(Context& context);
    virtual ~BaseComponent() = default;

    virtual void render();

    virtual void content() = 0;

protected:

    Context& context_;

};

} // namespace imgui
} // namespace mixi
