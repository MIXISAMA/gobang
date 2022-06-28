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

    enum class Usage
    {
        STREAM_DRAW  = GL_STREAM_DRAW,
        STREAM_READ  = GL_STREAM_READ,
        STREAM_COPY  = GL_STREAM_COPY,
        STATIC_DRAW  = GL_STATIC_DRAW,
        STATIC_READ  = GL_STATIC_READ,
        STATIC_COPY  = GL_STATIC_COPY,
        DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
        DYNAMIC_READ = GL_DYNAMIC_READ,
        DYNAMIC_COPY = GL_DYNAMIC_COPY,
    };

    Buffer(GLenum target, GLsizeiptr bytes, void* data, Usage usage);
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
        Usage usage,
        GLsizei count,
        const std::vector<Descriptor>& descriptors = {}
    );

    const GLsizei count() const;

    std::vector<Descriptor>& descriptors();
    const std::vector<Descriptor>& descriptors() const;

protected:

    const GLsizei count_;

    std::vector<Descriptor> descriptors_;

};

class ElementBuffer : public Buffer
{

public:

    enum class Type
    {
        UNSIGNED_BYTE  = GL_UNSIGNED_BYTE,
        UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
        UNSIGNED_INT   = GL_UNSIGNED_INT,
    };

    ElementBuffer(
        GLsizeiptr bytes,
        void* data,
        Type type = Type::UNSIGNED_INT,
        Usage usage = Usage::STATIC_DRAW
    );

    const GLsizei count() const;
    const Type type() const;

protected:

    GLsizei count_;
    const Type type_;

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
        Usage usage = Usage::DYNAMIC_DRAW
    );

    GLuint binding_point() const;

protected:

    const GLuint binding_point_;

    static GLuint Uniform_Buffer_Count_;

};

} // namespace gl
} // namespace mixi