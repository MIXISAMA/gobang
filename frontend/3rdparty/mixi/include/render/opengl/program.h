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

    using Ptr = std::shared_ptr<Program>;

    Program(const std::initializer_list<Shader>& shaders);
    ~Program();

    void use() const;

    GLint get_uniform_location(const char* name) const;

    void set_uniform_int  (GLint location, int          value) const;
    void set_uniform_float(GLint location, float        value) const;
    void set_uniform_vec3 (GLint location, const float* value) const;
    void set_uniform_mat4 (GLint location, const float* value) const;

    void set_uniform_blocks_binding_points(
        const std::initializer_list<std::string>& names
    ) const;

private:

    void check_link_errors_() const;

};

} // namespace gl
} // namespace mixi
