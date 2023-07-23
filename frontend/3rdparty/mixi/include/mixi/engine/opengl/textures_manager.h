#pragma once
#include "mixi/pch.h"

#include "mixi/render/opengl/texture.h"


namespace mixi
{
namespace gl
{
namespace eng
{

class TexturesManager
{

public:

    static std::shared_ptr<Texture2D> Get(const std::filesystem::path& filepath);
    static void Remove(const std::filesystem::path& filepath);
    static void Clear();

protected:

    static std::map<std::filesystem::path, std::shared_ptr<Texture2D>> Textures_;

};

} // namespace eng
} // namespace gl
} // namespace mixi
