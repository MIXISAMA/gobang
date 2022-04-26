#pragma once
#include "pch.h"

#include "render/opengl/bind.h"

namespace mixi
{
namespace gl
{

class Texture2D : public Bindable
{
public:

    enum class Format : GLint
    {
        RED             = GL_RED,
        RG              = GL_RG,
        RGB             = GL_RGB,
        BGR             = GL_BGR,
        RGBA            = GL_RGBA,
        BGRA            = GL_BGRA,
        RED_INTEGER     = GL_RED_INTEGER,
        RG_INTEGER      = GL_RG_INTEGER,
        RGB_INTEGER     = GL_RGB_INTEGER,
        BGR_INTEGER     = GL_BGR_INTEGER,
        RGBA_INTEGER    = GL_RGBA_INTEGER,
        BGRA_INTEGER    = GL_BGRA_INTEGER,
        STENCIL_INDEX   = GL_STENCIL_INDEX,
        DEPTH_COMPONENT = GL_DEPTH_COMPONENT,
        DEPTH_STENCIL   = GL_DEPTH_STENCIL,
    };

    using Ptr = std::shared_ptr<Texture2D>;

    Texture2D();
    ~Texture2D();

    void bind() const override;
    void unbind() const override;

    void update_image(
        GLsizei width,
        GLsizei height,
        Format format,
        const void *data
    );

};

} // namespace gl
} // namespace mixi
