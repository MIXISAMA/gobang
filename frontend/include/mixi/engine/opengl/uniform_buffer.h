#pragma once
#include "pch.h"

#include "render/opengl/buffer.h"

namespace mixi
{
namespace gl
{
namespace eng
{

class CameraUniformBuffer : public UniformBuffer
{

public:

    CameraUniformBuffer(const char* name = "Camera");

    const char* name() const;

    void update_projection(const float* p) const;
    void update_view      (const float* v) const;

protected:

    const char* name_;

};

} // namespace eng
} // namespace gl
} // namespace mixi