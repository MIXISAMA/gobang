#pragma once
#include "mixi/pch.h"

#include "mixi/render/opengl/buffer.h"

namespace mixi
{
namespace gl
{
namespace eng
{

class CameraUniformBuffer : public UniformBuffer
{

public:

    using Ptr = std::shared_ptr<CameraUniformBuffer>;

    CameraUniformBuffer(const char* name = "Camera");

    const char* name() const;

    void update_projection(const float* p) const;
    void update_view      (const float* v) const;

protected:

    const char* name_;

};

class PointLightUniformBuffer : public UniformBuffer
{

public:
    // Todo: not a good design
    enum class Part
    {
        POSITION,
        AMBIENT,
        DIFFUSE,
        SPECULAR,
        ATTENUATION,
    };

    PointLightUniformBuffer(const char* name = "PointLight");

    const char* name() const;

    void update(Part part, const void* val) const;

protected:

    const char* name_;

private:

    // Todo: not a good design
    static const size_t bytes_[];
    static const size_t std140_offset_[];

};

} // namespace eng
} // namespace gl
} // namespace mixi