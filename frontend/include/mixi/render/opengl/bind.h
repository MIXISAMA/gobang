#pragma once
#include "pch.h"

namespace mixi
{
namespace gl
{

class Bindable
{

public:

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    GLuint id();

protected:

    GLuint id_;

};

class Bind
{

public:

    Bind(const Bindable& bindable);
    ~Bind();

protected:

    const Bindable& bindable_;

};

} // namespace gl
} // namespace mixi
