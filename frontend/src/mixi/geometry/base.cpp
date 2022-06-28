#include "geometry/base.h"

namespace mixi
{
namespace geo
{

const std::vector<glm::vec3>& Vertices::vertices() const
{
    return vertices_;
}

const std::vector<glm::uvec3>& Indices::indices() const
{
    return indices_;
}

} // namespace geo
} // namespace mixi
