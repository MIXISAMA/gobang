#include "mixi/render/opengl/shader.h"
#include "mixi/core/log.h"

namespace mixi
{
namespace gl
{

Shader::Shader(const std::filesystem::path& path, GLenum shader_type)
{
    id_ = glCreateShader(shader_type);
    compile_(path);
    check_compile_errors_();
}

Shader::~Shader()
{
    glDeleteShader(id_);
}

void Shader::compile_(const std::filesystem::path& path) const
{
    std::string code_string;
    std::ifstream shader_file;
    shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    std::stringstream shader_stream;

    shader_file.open(path);
    shader_stream << shader_file.rdbuf();
    shader_file.close();

    code_string = shader_stream.str();
    const char* code = code_string.c_str();

    glShaderSource(id_, 1, &code, NULL);
    glCompileShader(id_);
}

void Shader::check_compile_errors_() const
{
    GLint success = 0;
    GLchar info_log[1024];

    glGetShaderiv(id_, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(id_, 1024, NULL, info_log);
        Log::Error(info_log);
        throw std::runtime_error(info_log);
    }
}

} // namespace gl
} // namespace mixi
