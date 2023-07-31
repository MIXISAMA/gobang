#pragma once
#include "pch.h"
#include "mixi/render/opengl/program.h"
#include "mixi/engine/opengl/model.h"
#include "mixi/geometry/camera.h"

namespace mixi
{
namespace gobang
{

class ChesspieceProgram : public gl::Program
{

public:

    ChesspieceProgram(
        const std::shared_ptr<geo::Camera> camera,
        const gl::Shader& vertex_shader,
        const gl::Shader& fragment_shader
    );

    void set_projection(const glm::mat4& projection);
    void set_model(const glm::mat4& model);
    void set_color(const glm::vec3& color);
    gl::VertexArray::Ptr gen_vertex_array(gl::VertexBuffer::Ptr vbo);

    void prepare_use() const;

private:

    const std::shared_ptr<geo::Camera> camera_;

    GLint location_view_pos_;
    GLint location_model_;
    GLint location_projection_;
    GLint location_view_;
    GLint location_color_;

};

} // namespace gobang
} // namespace mixi
