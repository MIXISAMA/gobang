#include "render/opengl/bind.h"

namespace mixi
{
namespace gl
{

GLuint Idable::id() const
{
    return id_;
}

Bind::Bind(const Bindable& bindable) :
    bindable_(bindable)
{
    bindable.bind();
}

Bind::~Bind()
{
    bindable_.unbind();
}

} // namespace gl
} // namespace mixi