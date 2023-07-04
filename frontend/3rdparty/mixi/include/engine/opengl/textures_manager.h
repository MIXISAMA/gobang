#include "pch.h"

#include "render/opengl/texture.h"


namespace mixi
{
namespace gl
{
namespace eng
{

class TexturesManager
{

public:

    Texture2D::Ptr get_or_create(const std::filesystem::path& filepath);

protected:

    std::map<std::filesystem::path, Texture2D::Ptr> textures_;

};

} // namespace eng
} // namespace gl
} // namespace mixi
