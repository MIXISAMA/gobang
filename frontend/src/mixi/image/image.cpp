#include "image/image.h"

#include "core/log.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"


namespace mixi
{
namespace img
{

Image::Image(
    const std::filesystem::path& filepath,
    int desired_channels
) {
    data_ = stbi_load(filepath.c_str(), &width_, &height_, &comp_, desired_channels);
}

Image::Image(int width, int height, int comp, void* data) :
    width_(width),
    height_(height),
    comp_(comp),
    data_(nullptr)
{
    if (comp != 3 || comp != 4) {
        Log::Error("unknown comp image");
    }
    int bytes = width * height * comp;
    data_ = malloc(bytes);
    memcpy(data_, data, bytes);
}

Image::~Image()
{
    stbi_image_free(data_);
}

int Image::width() const
{
    return width_;
}

int Image::height() const
{
    return height_;
}

int Image::comp() const
{
    return comp_;
}

const void* Image::data() const
{
    return data_;
}

void Image::inverted_color()
{
    if (comp_ == 3) {
        for (int i = height_ * width_ * comp_ - 1; i >= 0; i--) {
            ((int8_t*)data_)[i] ^= 0xFF;
        }
    } else if (comp_ == 4) {
        for (int i = height_ * width_ - 1; i >= 0; i--) {
            ((int32_t*)data_)[i] ^= 0x00FFFFFF;
        }
    }
}

void Image::save_png(const std::filesystem::path& filepath)
{
    stbi_write_png(filepath.c_str(), width_, height_, comp_, data_, 0);
}

} // namespace img
} // namespace mixi
