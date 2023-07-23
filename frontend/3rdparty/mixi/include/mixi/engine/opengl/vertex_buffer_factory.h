#pragma once
#include "mixi/pch.h"

#include "mixi/render/opengl/buffer.h"
#include "mixi/geometry/base.h"

namespace mixi
{
namespace gl
{
namespace eng
{



VertexBuffer* New_Vertex_Buffer(const geo::Vertices& v)
{
    return new VertexBuffer(
        v.vertices().size() * sizeof(glm::vec3),
        (void*)v.vertices().data(),
        GL_STATIC_DRAW,
        v.vertices().size(),
        {gl::VertexBuffer::Descriptor{
            .size       = 3,
            .type       = GL_FLOAT,
            .normalized = GL_FALSE,
            .stride     = sizeof(glm::vec3),
            .pointer    = (void*)0
        }}
    );
}

} // namespace eng
} // namespace gl
} // namespace mixi
