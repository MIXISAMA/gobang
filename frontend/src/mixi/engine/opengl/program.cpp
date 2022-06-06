#include  "engine/opengl/program.h"

namespace mixi
{
namespace gl
{
namespace eng
{

void ModelProgram::update_model(const float* model_matrix) const
{
    set_uniform_mat4(location_model_, model_matrix);
}

} // namespace eng
} // namespace gl
} // namespace mixi
