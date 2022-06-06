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

    std::vector<Descriptor>& descriptors();

protected:

    const GLsizei count_;

    std::vector<Descriptor> descriptors_;

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

    void storage(
        int width,
        int height,
        GLenum internal_format = GL_DEPTH24_STENCIL8
    );

};

class FrameBuffer : public Bindable
{

public:

    FrameBuffer(int width = 1, int height = 1);
    ~FrameBuffer();

    void bind() const override;
    void unbind() const override;

    void resize(int width, int height);

    const Texture2D& texture() const;

protected:

    Texture2D texture_;
    RenderBuffer render_buffer_;

};

class UniformBuffer : public Buffer
{

public:

    UniformBuffer(
        GLsizeiptr bytes,
        void* data = nullptr,
        GLenum usage = GL_DYNAMIC_DRAW
    );

    GLuint binding_point() const;

protected:

    const GLuint binding_point_;

    static GLuint Uniform_Buffer_Count_;

};

} // namespace gl
} // namespace mixi