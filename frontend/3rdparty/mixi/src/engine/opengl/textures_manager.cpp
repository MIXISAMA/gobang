#include "engine/opengl/textures_manager.h"
#include "image/image.h"

namespace mixi
{
namespace gl
{
namespace eng
{

Texture2D::Ptr TexturesManager::get_or_create(const std::filesystem::path& filepath)
{
    auto iter = textures_.find(filepath);
    if (iter != textures_.end()) {
        return iter->second;
    }

    img::Image img(filepath);
    Texture2D::Ptr tex(new Texture2D);
    tex->update_image(
        img.width(),
        img.height(),
        gl::Texture2D::Format::RGBA,
        img.data()
    );
    textures_[filepath] = tex;
    return tex;
}

} // namespace eng
} // namespace gl
} // namespace mixi
