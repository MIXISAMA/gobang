#pragma once
#include "pch.h"

#include "render/opengl/bind.h"
#include "render/opengl/buffer.h"

namespace mixi
{
namespace gl
{

class VertexArray : public Bindable
{

public:

    enum class Mode : GLenum
    {
        POINTS                   = GL_POINTS,
        LINE_STRIP               = GL_LINE_STRIP,
        LINE_LOOP                = GL_LINE_LOOP,
        LINES                    = GL_LINES,
        LINE_STRIP_ADJACENCY     = GL_LINE_STRIP_ADJACENCY,
        LINES_ADJACENCY          = GL_LINES_ADJACENCY,
        TRIANGLE_STRIP           = GL_TRIANGLE_STRIP,
        TRIANGLE_FAN             = GL_TRIANGLE_FAN,
        TRIANGLES                = GL_TRIANGLES,
        TRIANGLE_STRIP_ADJACENCY = GL_TRIANGLE_STRIP_ADJACENCY,
        TRIANGLES_ADJACENCY      = GL_TRIANGLES_ADJACENCY,
        PATCHES                  = GL_PATCHES,
    };

    VertexArray(Mode mode = Mode::TRIANGLES);
    ~VertexArray();

    void bind() const override;
    void unbind() const override;

    Mode mode() const;
    void mode(Mode m);

    void bind_vertex_buffer(
        const VertexBuffer::Ptr& vertex_buffer,
        const std::vector<std::pair<int, int>>& location_descriptor_map
    );
    void bind_element_buffer(
        const ElementBuffer::Ptr& element_buffer
    );

    void draw_arrays(GLint first, GLsizei count) const;
    void draw_arrays() const;

    void draw_elements(GLsizei first_byte, GLsizei count) const;
    void draw_elements() const;

    void draw(GLint first, GLsizei count) const;
    void draw() const;

protected:

    Mode mode_;

    VertexBuffer::Ptr vertex_buffer_;
    ElementBuffer::Ptr element_buffer_;

    static int Cal_Elements_Count_(Mode m, int count);

};

} // namespace gl
} // namespace mixi
