#include "render/opengl/program.h"

namespace mixi
{
namespace gl
{

template<typename... Args>
Program::Program(const Shader& shader, Args&&... shaders)
{
    id_ = glCreateProgram();
    attach_shaders_(shader, shaders...);
    glLinkProgram(id_);
    check_link_errors_();
}

Program::~Program()
{
    glDeleteProgram(id_);
}

void Program::use() const
{
    glUseProgram(id_);
}

template<typename... Args>
void Program::attach_shaders_(const Shader& shader, Args&&... shaders) const
{
    glAttachShader(id_, shader.id());
    attach_shaders_(shaders...);
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

void Program::set_uniform_int(GLint location, int value) const
{
    glProgramUniform1i(id_, location, value);
}

void Program::set_uniform_float(GLint location, float value) const
{
    glProgramUniform1f(id_, location, value);
}

void Program::set_uniform_vec3(GLint location, float* value) const
{
    glProgramUniform3fv(id_, location, 1, value);
}

void Program::set_uniform_mat4(GLint location, float* value) const
{
    glProgramUniformMatrix4fv(id_, location, 1, GL_FALSE, value);
}

} // namespace gl
} // namespace mixi
