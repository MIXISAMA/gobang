#include "geometry/plane.h"

namespace mixi
{
namespace geo
{

Plane::Plane() :
    vertices_{
        {-1.0,  1.0, 0.0},
        {-1.0, -1.0, 0.0},
        { 1.0, -1.0, 0.0},
        { 1.0,  1.0, 0.0},
    },
    indices_{
        {0, 1, 2},
        {0, 2, 3},
    }
{

}

const std::vector<glm::vec3>& Plane::vertices()
{
    return vertices_;
}

const std::vector<glm::ivec3>& Plane::indices()
{
    return indices_;
}

} // namespace geo
} // namespace mixi
