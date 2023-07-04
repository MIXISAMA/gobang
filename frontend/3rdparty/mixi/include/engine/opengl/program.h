#pragma once
#include "pch.h"
#include "render/opengl/shader.h"
#include "render/opengl/program.h"
#include "engine/opengl/uniform_buffer.h"


namespace mixi
{
namespace gl
{
namespace eng
{

class ModelCameraProgram : public Program
{

public:

    using Ptr = std::shared_ptr<ModelCameraProgram>;

    ModelCameraProgram(
        const CameraUniformBuffer::Ptr& camera_uniform_buffer,
        const Shader& vertex_shader,
        const Shader& fragment_shader
    );

    void update_model(const float* model_matrix) const;

protected:

    GLint location_model_;
    CameraUniformBuffer::Ptr camera_uniform_buffer_;

};

} // namespace eng
} // namespace gl
} // namespace mixi
