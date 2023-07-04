#pragma once
#include "pch.h"

namespace mixi
{
namespace img
{

class Image
{

public:

    using Ptr = std::shared_ptr<Image>;

    Image(
        const std::filesystem::path& filepath,
        int desired_channels = 4
    );
    Image(int width, int height, int comp, void* data);

    ~Image();

    int width() const;
    int height() const;
    int comp() const;
    const void* data() const;

    void inverted_color();

    void save_png(const std::filesystem::path& filepath);

protected:

    int width_;
    int height_;
    int comp_;
    void* data_;

};

} // namespace img
} // namespace mixi
