#pragma once
#include "pch.h"
#include "engine/opengl/program/model.h"
#include "engine/opengl/uniform_buffer/camera.h"

namespace mixi
{
namespace gl
{
namespace eng
{

class CameraModelProgram : public ModelProgram
{

public:

    template<typename... Args>
    CameraModelProgram(
        const char* model_uniform_name,
        const CameraUniformBuffer& camera_uniform_buffer,
        const Args&... shaders
    ) :
        ModelProgram(model_uniform_name, shaders...),
        camera_uniform_buffer_(camera_uniform_buffer)
    {
        bind_uniform_block(
            camera_uniform_buffer_.name(),
            camera_uniform_buffer_.binding_point()
        );
    }

protected:

    const CameraUniformBuffer& camera_uniform_buffer_;

};

} // namespace eng
} // namespace gl
} // namespace mixi
