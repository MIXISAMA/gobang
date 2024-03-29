#pragma once
#include "mixi/pch.h"

namespace mixi
{
namespace gui
{

class BaseComponent
{
public:

    virtual void render();

protected:

    virtual void content() = 0;

};

template<class T>
concept TBaseComponent = std::is_base_of<BaseComponent, T>::value;

} // namespace gui
} // namespace mixi
