#include "drawable/program.h"

namespace mixi
{
namespace gobang
{

ChessboardProgram::ChessboardProgram(
    const std::shared_ptr<geo::Camera> camera,
    const gl::Shader& vertex_shader,
    const gl::Shader& fragment_shader
) :
    Program({vertex_shader, fragment_shader}),
    camera_(camera),
    location_material_shininess_(get_uniform_location("uMaterial.shininess")),
    location_view_pos_(get_uniform_location("uViewPos")),
    location_model_(get_uniform_location("uModel")),
    location_projection_(get_uniform_location("uProjection")),
    location_view_(get_uniform_location("uView"))
{
    set_model(glm::mat4(1.0f));

    set_uniform_int(get_uniform_location("uMaterial.diffuse"),  0); // GL_TEXTURE0
    set_uniform_int(get_uniform_location("uMaterial.specular"), 1); // GL_TEXTURE1

    set_uniform_vec3(get_uniform_location("uLight.position"), glm::vec3(1.2f, 1.0f, 2.0f));
    set_uniform_vec3(get_uniform_location("uLight.ambient"),  glm::vec3(0.2f, 0.2f, 0.2f));
    set_uniform_vec3(get_uniform_location("uLight.diffuse"),  glm::vec3(0.5f, 0.5f, 0.5f));
    set_uniform_vec3(get_uniform_location("uLight.specular"), glm::vec3(1.0f, 1.0f, 1.0f));
}


void ChessboardProgram::set_material(const gl::eng::Material* material)
{
    if (material == nullptr) {
        return;
    }
    texture_diffuse_ = material->diffuse;
    texture_specular_ = material->specular;
    set_uniform_int(location_material_shininess_, material->shininess);
}

void ChessboardProgram::set_projection(const glm::mat4& projection)
{
    set_uniform_mat4(location_projection_, projection);
}

void ChessboardProgram::set_model(const glm::mat4& model)
{
    set_uniform_mat4(location_model_, model);
}

gl::VertexArray::Ptr ChessboardProgram::gen_vertex_array(
    gl::VertexBuffer::Ptr vbo
) {
    std::vector<int> idx(vbo->descriptors().size(), -1);
    for (int i = 0; i < vbo->descriptors().size(); i++) {
        const gl::VertexBuffer::Descriptor& d = vbo->descriptors()[i];
        if (d.name == "vertex") {
            idx[i] = 0;
        }
        else if (d.name == "normal") {
            idx[i] = 1;
        }
        else if (d.name.substr(0, 8) == "texture") {
            idx[i] = 2;
        }
    }
    gl::VertexArray::Ptr vao = std::make_shared<gl::VertexArray>();
    vao->bind_vertex_buffer(vbo, idx);
    return vao;
}

void ChessboardProgram::prepare_use() const
{
    if (texture_diffuse_.get() != nullptr) {
        glActiveTexture(GL_TEXTURE0);
        texture_diffuse_->bind();
    }
    if (texture_specular_.get() != nullptr) {
        glActiveTexture(GL_TEXTURE1);
        texture_specular_->bind();
    }
    use();
    set_uniform_vec3(location_view_pos_, camera_->position());
    set_uniform_mat4(location_view_, camera_->view_matrix());
}


} // namespace gobang
} // namespace mixi
