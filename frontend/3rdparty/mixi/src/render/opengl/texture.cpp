#include "mixi/render/opengl/texture.h"

namespace mixi
{
namespace gl
{

Texture2D::Texture2D(GLint min_mag_filter, GLint warp_st) :
    width_(0),
    height_(0)
{
    glGenTextures(1, &id_);
    Bind b(*this);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, min_mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, warp_st);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, warp_st);
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &id_);
}

void Texture2D::bind() const
{
    glBindTexture(GL_TEXTURE_2D, id_);
}

void Texture2D::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLsizei Texture2D::width() const
{
    return width_;
}

GLsizei Texture2D::height() const
{
    return height_;
}

void Texture2D::update_image(
    GLsizei width,
    GLsizei height,
    Format format,
    const void *data
) {
    Bind b(*this);

#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        (GLuint)format,
        width,
        height,
        0,
        (GLuint)format,
        GL_UNSIGNED_BYTE,
        data
    );

    width_  = width;
    height_ = height;
}

} // namespace gl
} // namespace mixi
