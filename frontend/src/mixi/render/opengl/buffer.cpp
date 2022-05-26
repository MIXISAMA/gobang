#include "render/opengl/buffer.h"

namespace mixi
{
namespace gl
{

Buffer::Buffer(GLenum target, GLsizeiptr bytes, void *data, GLenum usage) :
    target_(target)
{

    glGenBuffers(1, &id_);

    Bind bind(*this);
    glBufferData(target_, bytes, data, usage);

}

Buffer::~Buffer()
{
    glDeleteBuffers(1, &id_);
}

void Buffer::bind() const
{
    glBindBuffer(target_, id_);
}

void Buffer::unbind() const
{
    glBindBuffer(target_, 0);
}

VertexBuffer::VertexBuffer(
    GLsizeiptr bytes,
    void* data,
    GLenum usage,
    GLsizei count,
    const std::vector<Descriptor>& descriptors
) :
    Buffer(GL_ARRAY_BUFFER, bytes, data, usage),
    count_(count),
    descriptors_(descriptors)
{

}

const GLsizei VertexBuffer::count() const
{
    return count_;
}

const std::vector<VertexBuffer::Descriptor>& VertexBuffer::descriptors() const
{
    return descriptors_;
}

ElementBuffer::ElementBuffer(
    GLsizeiptr bytes,
    void* data,
    GLenum type,
    GLenum usage
) :
    Buffer(GL_ELEMENT_ARRAY_BUFFER, bytes, data, usage),
    type_(type)
{

}

const GLenum ElementBuffer::type() const
{
    return type_;
}

RenderBuffer::RenderBuffer(int width, int height, GLenum internal_format)
{
    glGenRenderbuffers(1, &id_);
    storage(width, height, internal_format);
}

RenderBuffer::~RenderBuffer()
{
    glDeleteRenderbuffers(1, &id_);
}

void RenderBuffer::bind() const
{
    glBindRenderbuffer(GL_RENDERBUFFER, id_);
}

void RenderBuffer::unbind() const
{
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void RenderBuffer::storage(
    int width,
    int height,
    GLenum internal_format
) {
    Bind b(*this);
    glRenderbufferStorage(GL_RENDERBUFFER, internal_format, width, height);
}

FrameBuffer::FrameBuffer(int width, int height) :
    render_buffer_(width, height)
{
    texture_.update_image(width, height, Texture2D::Format::RGB, nullptr);

    glGenFramebuffers(1, &id_);

    Bind b(*this);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        texture_.id(),
        0
    );
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        render_buffer_.id()
    );

    GLenum state = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (state != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("frame buffer create error");
    }
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &id_);
}

void FrameBuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
}

void FrameBuffer::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::resize(int width, int height)
{
    texture_.update_image(width, height, Texture2D::Format::RGB, nullptr);
    render_buffer_.storage(width, height);
}

const Texture2D& FrameBuffer::texture() const
{
    return texture_;
}

} // namespace gl
} // namespace mixi
