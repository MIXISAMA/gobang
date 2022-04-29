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
    Program(const Shader& shader, Args&&... shaders);
    ~Program();

    void use() const;

    void set_uniform_int  (GLint location, int    value) const;
    void set_uniform_float(GLint location, float  value) const;
    void set_uniform_vec3 (GLint location, float* value) const;
    void set_uniform_mat4 (GLint location, float* value) const;

private:

    template<typename... Args>
    void attach_shaders_(const Shader& shader, Args&&... shaders) const;
    void check_link_errors_() const;

};

} // namespace gl
} // namespace mixi
