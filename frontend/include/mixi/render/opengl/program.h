#pragma once
#include "pch.h"

#include "render/opengl/shader.h"

namespace mixi
{
namespace gl
{

class Program : public Idable
{

public:

    template<typename... Args>
    Program(const Shader& shader, const Args&... shaders)
    {
        id_ = glCreateProgram();
        attach_shaders_(shader, shaders...);
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

            printf("Attribute #%d Type: %u Name: %s\n", i, type, name);
        }

        glGetProgramiv(id_, GL_ACTIVE_UNIFORMS, &count);
        printf("Active Uniforms: %d\n", count);

        for (GLint i = 0; i < count; i++)
        {
            glGetActiveUniform(id_, (GLuint)i, bufSize, &length, &size, &type, name);

            printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
        }

    }

    ~Program();

    void use() const;

    GLint get_uniform_location(const char* name) const;

    void set_uniform_int  (GLint location, int    value) const;
    void set_uniform_float(GLint location, float  value) const;
    void set_uniform_vec3 (GLint location, float* value) const;
    void set_uniform_mat4 (GLint location, float* value) const;

private:

    void attach_shaders_() const
    {

    }

    template<typename... Args>
    void attach_shaders_(const Shader& shader, const Args&... shaders) const
    {
        glAttachShader(id_, shader.id());
        attach_shaders_(shaders...);
    }

    void check_link_errors_() const;

};

} // namespace gl
} // namespace mixi
