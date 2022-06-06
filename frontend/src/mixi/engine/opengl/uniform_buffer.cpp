#include "engine/opengl/uniform_buffer.h"

namespace mixi
{
namespace gl
{
namespace eng
{


CameraUniformBuffer::CameraUniformBuffer(const char* name) : 
    UniformBuffer(sizeof(float) * 16 * 2),
    name_(name)
{

}

const char* CameraUniformBuffer::name() const
{
    return name_;
}

void CameraUniformBuffer::update_projection(const float* p) const
{
    Bind b(*this);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 16, p);
}

void CameraUniformBuffer::update_view(const float* v) const
{
    Bind b(*this);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 16, sizeof(float) * 16, v);
}

} // namespace eng
} // namespace gl
} // namespace mixi
