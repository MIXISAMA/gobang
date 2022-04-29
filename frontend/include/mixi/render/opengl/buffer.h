#pragma once
#include "pch.h"
#include "render/opengl/bind.h"

#include "render/opengl/texture.h"

namespace mixi
{
namespace gl
{

class Buffer : public Bindable
{

public:

    Buffer(GLenum target, GLsizeiptr bytes, void* data, GLenum usage);
    virtual ~Buffer();

    void bind() const override;
    void unbind() const override;

protected:

    GLenum target_;

};

class VertexBuffer : public Buffer
{

public:

    struct Descriptor
    {
        GLint size;
        GLenum type;
        GLboolean normalized;
        GLsizei stride;
        const void * pointer;
    };

    VertexBuffer(
        GLsizeiptr bytes,
        void* data,
        GLenum usage,
        GLsizei count,
        const std::vector<Descriptor>& descriptors = {}
    );

    const GLsizei count() const;

    const std::vector<Descriptor>& descriptors() const;

protected:

    const GLsizei count_;

    const std::vector<Descriptor> descriptors_;

};

class ElementBuffer : public Buffer
{

public:

    ElementBuffer(
        GLsizeiptr bytes,
        void* data,
        GLenum type = GL_UNSIGNED_INT,
        GLenum usage = GL_STATIC_DRAW
    );

    const GLenum type() const;

protected:

    const GLenum type_;

};

class RenderBuffer : public Bindable
{

public:

    RenderBuffer(
        int width,
        int height,
        GLenum internal_format = GL_DEPTH24_STENCIL8
    );
    ~RenderBuffer();

    void bind() const override;
    void unbind() const override;

};

class FrameBuffer : public Bindable
{

public:

    FrameBuffer(int width, int height);
    ~FrameBuffer();

    void bind() const override;
    void unbind() const override;

    const Texture2D& texture() const;

protected:

    Texture2D texture_;
    RenderBuffer render_buffer_;

};

} // namespace gl
} // namespace mixi