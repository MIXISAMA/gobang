#include "render/opengl/texture.h"

namespace mixi
{
namespace gl
{

Texture2D::Texture2D()
{
    glGenTextures(1, &id_);
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

void Texture2D::update_image(
    GLsizei width,
    GLsizei height,
    Format format,
    const void *data
) {
    Bind bind(*this);

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
}

} // namespace gl
} // namespace mixi
