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

    VertexArray(GLenum mode = GL_TRIANGLES);
    ~VertexArray();

    void bind() const override;
    void unbind() const override;

    GLenum mode() const;
    void mode(GLenum m);

    void bind_vertex_buffer(
        const VertexBuffer& vertex_buffer,
        const std::vector<std::pair<int, int>>& location_descriptor_map
    );
    void bind_element_buffer(
        const ElementBuffer& element_buffer
    );

    void draw_arrays(GLint first, GLsizei count) const;
    void draw_arrays() const;

    void draw_elements(GLsizei first_byte, GLsizei count) const;
    void draw_elements() const;

    void draw(GLint first, GLsizei count) const;
    void draw() const;

protected:

    GLenum mode_;

    bool has_vertex_buffer_;
    bool has_element_buffer_;
    GLsizei default_arrays_count_;
    GLsizei default_elements_count_;
    GLenum default_elements_type_;

};

} // namespace gl
} // namespace mixi
