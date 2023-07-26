#pragma once
#include "pch.h"
#include "mixi/render/opengl/program.h"
#include "mixi/engine/opengl/uniform_buffer.h"
#include "mixi/engine/opengl/model.h"
#include "mixi/geometry/camera.h"

namespace mixi
{
namespace gobang
{

class ChessboardProgram : public gl::Program
{

public:

    ChessboardProgram(
        const std::shared_ptr<geo::Camera> camera,
        const gl::Shader& vertex_shader,
        const gl::Shader& fragment_shader
    );

    void set_material(const gl::eng::Material* material);
    void set_projection(const glm::mat4& projection);
    void set_model(const glm::mat4& model);
    gl::VertexArray::Ptr gen_vertex_array(gl::VertexBuffer::Ptr vbo);

    void prepare_use() const;

private:

    const std::shared_ptr<geo::Camera> camera_;

    std::shared_ptr<gl::Texture2D> texture_diffuse_;
    std::shared_ptr<gl::Texture2D> texture_normals_;

    GLint location_material_shininess_;
    GLint location_view_pos_;
    GLint location_model_;
    GLint location_projection_;
    GLint location_view_;

};

} // namespace gobang
} // namespace mixi
