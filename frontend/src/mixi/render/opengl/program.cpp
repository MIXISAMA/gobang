#include "render/opengl/program.h"

namespace mixi
{
namespace gl
{

Program::~Program()
{
    glDeleteProgram(id_);
}

void Program::use() const
{
    glUseProgram(id_);
}

void Program::check_link_errors_() const
{
    GLint success;
    GLchar info_log[1024];

    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id_, 1024, NULL, info_log);
        throw std::runtime_error(info_log);
    }
}

GLint Program::get_uniform_location(const char* name) const
{
    return glGetUniformLocation(id_, name);
}

void Program::set_uniform_int(GLint location, int value) const
{
    use();
    glUniform1i(location, value);
}

void Program::set_uniform_float(GLint location, float value) const
{
    use();
    glUniform1f(location, value);
}

void Program::set_uniform_vec3(GLint location, const float* value) const
{
    use();
    glUniform3fv(location, 1, value);
}

void Program::set_uniform_mat4(GLint location, const float* value) const
{
    use();
    glUniformMatrix4fv(location, 1, GL_FALSE, value);
}

void Program::bind_uniform_block(const char* name, GLuint binding_point) const
{
    GLuint index = glGetUniformBlockIndex(id_, name);
    glUniformBlockBinding(id_, index, binding_point);
}



} // namespace gl
} // namespace mixi
