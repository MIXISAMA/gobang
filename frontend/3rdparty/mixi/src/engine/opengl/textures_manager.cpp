#include "mixi/engine/opengl/textures_manager.h"
#include "mixi/image/image.h"

namespace mixi
{
namespace gl
{
namespace eng
{

std::shared_ptr<Texture2D> TexturesManager::Get(const std::filesystem::path& filepath)
{
    auto iter = Textures_.find(filepath);
    if (iter != Textures_.end()) {
        return iter->second;
    }

    img::Image img(filepath);
    std::shared_ptr<Texture2D> tex(new Texture2D);
    tex->update_image(
        img.width(),
        img.height(),
        gl::Texture2D::Format::RGBA,
        img.data()
    );
    Textures_[filepath] = tex;
    return tex;
}

void TexturesManager::Clear()
{
    Textures_.clear();
}

std::map<std::filesystem::path, std::shared_ptr<Texture2D>> TexturesManager::Textures_;

} // namespace eng
} // namespace gl
} // namespace mixi
