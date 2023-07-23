#include  "mixi/engine/opengl/program.h"

namespace mixi
{
namespace gl
{
namespace eng
{

ModelCameraProgram::ModelCameraProgram(
    const CameraUniformBuffer::Ptr& camera_uniform_buffer,
    GLuint binding_point,
    const Shader& vertex_shader,
    const Shader& fragment_shader
) :
    Program({vertex_shader, fragment_shader}),
    camera_uniform_buffer_(camera_uniform_buffer),
    location_model_(get_uniform_location("model"))
{
    set_uniform_blocks_binding_point(
        camera_uniform_buffer_->name(),
        binding_point
    );
    camera_uniform_buffer_->binding_point(binding_point);
}

void ModelCameraProgram::update_model(const float* model_matrix) const
{
    set_uniform_mat4(location_model_, model_matrix);
}

} // namespace eng
} // namespace gl
} // namespace mixi
