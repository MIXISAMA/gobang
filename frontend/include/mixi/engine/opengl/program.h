#pragma once
#include "pch.h"
#include "render/opengl/program.h"
#include "engine/opengl/uniform_buffer.h"

namespace mixi
{
namespace gl
{
namespace eng
{

class ModelProgram : public Program
{

public:

    template<typename... Args>
    ModelProgram(
        const char* model_uniform_name,
        const Args&... shaders
    ) :
        Program(shaders...),
        location_model_(get_uniform_location(model_uniform_name))
    {

    }

    void update_model(const float* model_matrix) const;

protected:

    GLint location_model_;

};

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
