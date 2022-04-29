#pragma once
#include "pch.h"

#include "bind.h"
#include "core/exception.h"

namespace mixi
{
namespace gl
{

class Shader : public Idable
{

public:

    using Ptr = std::shared_ptr<Shader>;

    Shader(const std::filesystem::path& path, GLenum shader_type);
    ~Shader();

private:

    void compile_(const std::filesystem::path& path) const;
    void check_compile_errors_() const;

};

} // namespace gl
} // namespace mixi
