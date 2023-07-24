#include "mixi/render/opengl/program.h"

namespace mixi
{
namespace gl
{

Program::Program(const std::initializer_list<Shader>& shaders)
{
    id_ = glCreateProgram();
    for (const Shader& shader : shaders) {
        glAttachShader(id_, shader.id());
    }
    glLinkProgram(id_);
    check_link_errors_();

    GLint count;

    GLint size; // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)

    const GLsizei bufSize = 16; // maximum name length
    GLchar name[bufSize]; // variable name in GLSL
    GLsizei length; // name length

    glGetProgramiv(id_, GL_ACTIVE_ATTRIBUTES, &count);
    printf("Active Attributes: %d\n", count);

    for (GLint i = 0; i < count; i++)
    {
        glGetActiveAttrib(id_, (GLuint)i, bufSize, &length, &size, &type, name);
        printf("Attribute #%d Type: %u Size: %u Name: %s\n", i, type, size, name);
    }

    glGetProgramiv(id_, GL_ACTIVE_UNIFORMS, &count);
    printf("Active Uniforms: %d\n", count);

    for (GLint i = 0; i < count; i++)
    {
        glGetActiveUniform(id_, (GLuint)i, bufSize, &length, &size, &type, name);
        printf("Uniform #%d Type: %u Size: %u Name: %s\n", i, type, size, name);
    }
}

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

void Program::set_uniform_vec3(GLint location, const glm::vec3& value) const
{
    use();
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void Program::set_uniform_mat4(GLint location, const float* value) const
{
    use();
    glUniformMatrix4fv(location, 1, GL_FALSE, value);
}

void Program::set_uniform_mat4(GLint location, const glm::mat4& value) const
{
    use();
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

// void Program::set_uniform_blocks_binding_points(
//     const std::initializer_list<std::string>& names
// ) const
// {
//     int bp = 0;
//     for (const std::string& name : names) {
//         GLuint index = glGetUniformBlockIndex(id_, name.c_str());
//         glUniformBlockBinding(id_, index, bp);
//         bp++;
//     }
// }

void Program::set_uniform_blocks_binding_point(
    const std::string& name,
    GLuint binding_point
) const {
    GLuint index = glGetUniformBlockIndex(id_, name.c_str());
    glUniformBlockBinding(id_, index, binding_point);
}


} // namespace gl
} // namespace mixi
