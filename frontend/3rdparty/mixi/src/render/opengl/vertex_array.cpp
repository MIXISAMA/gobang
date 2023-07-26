#include "mixi/render/opengl/vertex_array.h"

namespace mixi
{
namespace gl
{

VertexArray::VertexArray(Mode mode) :
    mode_(mode)
{
    glGenVertexArrays(1, &id_);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &id_);
}

void VertexArray::bind() const
{
    glBindVertexArray(id_);
}

void VertexArray::unbind() const
{
    glBindVertexArray(0);
}

VertexArray::Mode VertexArray::mode() const
{
    return mode_;
}

void VertexArray::mode(Mode m)
{
    mode_ = m;
}

std::map<GLenum, int> TypeSize_ =
{
    {GL_FLOAT, sizeof(float)},
    {GL_INT,   sizeof(int  )},
};

void VertexArray::bind_vertex_buffer(
    VertexBuffer::Ptr vertex_buffer,
    const std::vector<std::string>& location_names
) {
    bind();
    vertex_buffer->bind();
    vertex_buffer_ = vertex_buffer;

    int stride = 0;
    for (const VertexBuffer::Descriptor d : vertex_buffer->descriptors()) {
        stride += d.size * TypeSize_[d.type];
    }

    for (int i = 0; i < location_names.size(); i++) {
        int offset = 0;
        for (const VertexBuffer::Descriptor d : vertex_buffer->descriptors()) {
            if (d.name == location_names[i]) {
                glVertexAttribPointer(i, d.size, d.type, d.normalized, stride, (void*)(intptr_t)offset);
                glEnableVertexAttribArray(i);
            }
            offset += d.size * TypeSize_[d.type];
        }
    }
}

void VertexArray::bind_element_buffer(
    const ElementBuffer::Ptr& element_buffer
) {
    bind();
    element_buffer->bind();

    element_buffer_ = element_buffer;
}

void VertexArray::draw_arrays(GLint first, GLsizei count) const
{
    bind();
    glDrawArrays((GLenum)mode_, first, count);
}

void VertexArray::draw_arrays() const
{
    draw_arrays(0, vertex_buffer_->count());
}

void VertexArray::draw_elements(GLsizei first_byte, GLsizei count) const
{
    bind();
    glDrawElements((GLenum)mode_, count, (GLenum)element_buffer_->type(), (void*)(uintptr_t)first_byte);
}

void VertexArray::draw_elements() const
{
    draw_elements(0, element_buffer_->count());
}

void VertexArray::draw(GLint first, GLsizei count) const
{
    if (element_buffer_) {
        draw_elements(first, count);
    } else if (vertex_buffer_) {
        draw_arrays(first, count);
    }
}

void VertexArray::draw() const
{
    if (element_buffer_) {
        draw_elements();
    } else if (vertex_buffer_) {
        draw_arrays();
    }
}

int VertexArray::Cal_Elements_Count_(Mode m, int count)
{
    switch (m)
    {
    case Mode::POINTS:
        return count;
    case Mode::LINE_STRIP:
        return count;
    case Mode::LINE_LOOP:
        return count;
    case Mode::LINES:
        return count;
    case Mode::LINE_STRIP_ADJACENCY:
        return count;
    case Mode::LINES_ADJACENCY:
        return count;
    case Mode::TRIANGLE_STRIP:
        return count;
    case Mode::TRIANGLE_FAN:
        return count;
    case Mode::TRIANGLES:
        return count;
    case Mode::TRIANGLE_STRIP_ADJACENCY:
        return count;
    case Mode::TRIANGLES_ADJACENCY:
        return count;
    case Mode::PATCHES:
        return count;
    }
    throw std::runtime_error("wrong mode");
}

} // namespace gl
} // namespace mixi
