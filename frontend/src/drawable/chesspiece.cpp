#include "drawable/chesspiece.h"


namespace mixi
{
namespace gobang
{

ChesspieceProgram::ChesspieceProgram(
    const std::shared_ptr<geo::Camera> camera,
    const gl::Shader& vertex_shader,
    const gl::Shader& fragment_shader
) :
    Program({vertex_shader, fragment_shader}),
    camera_(camera),
    location_view_pos_  (get_uniform_location("uViewPos")),
    location_model_     (get_uniform_location("uModel")),
    location_projection_(get_uniform_location("uProjection")),
    location_view_      (get_uniform_location("uView")),
    location_color_     (get_uniform_location("uColor"))
{
    set_model(glm::mat4(1.0f));

    set_uniform_vec3(get_uniform_location("uLight.position"), glm::vec3(3.0f, 3.0f, 3.0f));
    set_uniform_vec3(get_uniform_location("uLight.ambient"),  glm::vec3(0.2f, 0.2f, 0.2f));
    set_uniform_vec3(get_uniform_location("uLight.diffuse"),  glm::vec3(0.6f, 0.6f, 0.6f));
    set_uniform_vec3(get_uniform_location("uLight.specular"), glm::vec3(0.1f, 0.1f, 0.1f));

}

void ChesspieceProgram::set_projection(const glm::mat4& projection)
{
    set_uniform_mat4(location_projection_, projection);
}

void ChesspieceProgram::set_model(const glm::mat4& model)
{
    set_uniform_mat4(location_model_, model);
}

void ChesspieceProgram::set_color(const glm::vec3& color)
{
    set_uniform_vec3(location_color_, color);
}

gl::VertexArray::Ptr ChesspieceProgram::gen_vertex_array(
    gl::VertexBuffer::Ptr vbo
) {
    gl::VertexArray::Ptr vao = std::make_shared<gl::VertexArray>();
    vao->bind_vertex_buffer(vbo, {"vertex", "normal"});
    return vao;
}

void ChesspieceProgram::prepare_use() const
{
    use();
    set_uniform_vec3(location_view_pos_, camera_->position());
    set_uniform_mat4(location_view_, camera_->view_matrix());
}

} // namespace gobang
} // namespace mixi
