#include "render/opengl/vertex_array.h"

namespace mixi
{
namespace gl
{

VertexArray::VertexArray(GLenum mode) :
    mode_(mode),
    has_vertex_buffer_(false),
    has_element_buffer_(false),
    default_arrays_count_(0),
    default_elements_count_(0),
    default_elements_type_(GL_TRIANGLES)
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

GLenum VertexArray::mode() const
{
    return mode_;
}

void VertexArray::mode(GLenum m)
{
    mode_ = m;
}

void VertexArray::bind_vertex_buffer(
    const VertexBuffer& vertex_buffer,
    const std::vector<std::pair<int, int>>& location_descriptor_map
) {
    bind();
    Bind b(vertex_buffer);

    for (auto [location, idx]: location_descriptor_map) {
        const auto& ds = vertex_buffer.descriptors();
        const VertexBuffer::Descriptor& d = ds[idx];
        glVertexAttribPointer(location, d.size, d.type, d.normalized, d.stride, d.pointer);
        glEnableVertexAttribArray(location);
    }

    has_vertex_buffer_ = true;
    default_arrays_count_ = vertex_buffer.count();
}

void VertexArray::bind_element_buffer(
    const ElementBuffer& element_buffer
) {
    bind();
    element_buffer.bind();
    has_element_buffer_ = true;
    default_elements_type_ = element_buffer.type();
}

void VertexArray::draw_arrays(GLint first, GLsizei count) const
{
    bind();
    glDrawArrays(mode_, first, count);
}

void VertexArray::draw_arrays() const
{
    draw_arrays(0, default_arrays_count_);
}

void VertexArray::draw_elements(GLsizei first_byte, GLsizei count) const
{
    bind();
    glDrawElements(mode_, count, default_elements_type_, (void*)(uintptr_t)first_byte);
}

void VertexArray::draw_elements() const
{
    draw_elements(0, default_elements_count_);
}

void VertexArray::draw(GLint first, GLsizei count) const
{
    if (has_element_buffer_) {
        draw_elements(first, count);
    } else if (has_vertex_buffer_) {
        draw_arrays(first, count);
    }
}

void VertexArray::draw() const
{
    if (has_element_buffer_) {
        draw_elements();
    } else if (has_vertex_buffer_) {
        draw_arrays();
    }
}

} // namespace gl
} // namespace mixi
