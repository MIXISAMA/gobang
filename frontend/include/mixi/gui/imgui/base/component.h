#pragma once
#include "pch.h"

namespace mixi
{
namespace imgui
{

class BaseComponent
{
public:

    using Ptr = std::shared_ptr<BaseComponent>;

    virtual ~BaseComponent() = default;

    virtual void render();

    virtual void content() = 0;

};

} // namespace imgui
} // namespace mixi
