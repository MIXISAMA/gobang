#include "render/opengl/bind.h"

namespace mixi
{
namespace gl
{

GLuint Bindable::id()
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