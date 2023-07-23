#pragma once
#include "mixi/pch.h"

namespace mixi
{
namespace gl
{

class Idable
{

public:

    GLuint id() const;

protected:

    GLuint id_;

};

class Bindable : public Idable
{

public:

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

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
