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
    bind();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 16, p);
}

void CameraUniformBuffer::update_view(const float* v) const
{
    bind();
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 16, sizeof(float) * 16, v);
}

PointLightUniformBuffer::PointLightUniformBuffer(const char* name) :
    UniformBuffer(sizeof(float) * 4 * 5),
    name_(name)
{

}

const char* PointLightUniformBuffer::name() const
{
    return name_;
}

void PointLightUniformBuffer::update(Part part, const void* val) const
{
    UniformBuffer::update(std140_offset_[(int)part], bytes_[(int)part], val);
}

const size_t PointLightUniformBuffer::bytes_[] = {
    sizeof(glm::vec3),
    sizeof(glm::vec3),
    sizeof(glm::vec3),
    sizeof(glm::vec3),
    sizeof(float) * 3, 
};

const size_t PointLightUniformBuffer::std140_offset_[] = {
    sizeof(float) * 4 * 0,
    sizeof(float) * 4 * 1,
    sizeof(float) * 4 * 2,
    sizeof(float) * 4 * 3,
    sizeof(float) * 4 * 4, 
};

} // namespace eng
} // namespace gl
} // namespace mixi
