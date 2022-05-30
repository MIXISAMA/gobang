#pragma once
#include "pch.h"
#include "render/opengl/program.h"

namespace mixi
{
namespace gl
{
namespace eng
{

class ModelProgram : public Program
{

public:

    template<typename... Args>
    ModelProgram(
        const char* model_uniform_name,
        const Args&... shaders
    ) :
        Program(shaders...)
    {
        location_model_ = get_uniform_location(model_uniform_name);
    }

    void update_model(const float* model_matrix) const;

protected:

    GLint location_model_;

};

} // namespace eng
} // namespace gl
} // namespace mixi
