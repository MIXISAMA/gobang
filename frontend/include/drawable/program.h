#pragma once
#include "pch.h"
#include "mixi/engine/opengl/program.h"
#include "mixi/engine/opengl/uniform_buffer.h"

namespace mixi
{
namespace gobang
{

class PointLightProgram : public gl::eng::CameraModelProgram
{

public:

    PointLightProgram() :
        camera_uniform_buffer_(),
        gl::eng::CameraModelProgram(
            "model",
            camera_uniform_buffer_,
            gl::Shader("resource/glsl/point_light.vert", GL_VERTEX_SHADER),
            gl::Shader("resource/glsl/point_light.frag", GL_FRAGMENT_SHADER)
        )
    {
        bind_uniform_block(
            camera_uniform_buffer_.name(),
            camera_uniform_buffer_.binding_point()
        );
    }

protected:

    gl::eng::CameraUniformBuffer camera_uniform_buffer_;

};

} // namespace gobang
} // namespace mixi
