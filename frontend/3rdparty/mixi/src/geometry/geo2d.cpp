#include "geometry/geo2d.h"

namespace mixi
{
namespace geo
{

std::vector<glm::vec3> Geometry2D::vertices(Axis axis) const
{
    std::vector<glm::vec3> vs;
    switch (axis) {
    case Axis::X :
        for (glm::vec2 v2d : vertices_2d_) {
            vs.emplace_back(0.0f, v2d);
        }
        break;
    case Axis::Y :
        for (glm::vec2 v2d : vertices_2d_) {
            vs.emplace_back(v2d.x, 0.0f, v2d.y);
        }
        break;
    case Axis::Z :
        for (glm::vec2 v2d : vertices_2d_) {
            vs.emplace_back(v2d, 0.0f);
        }
        break;
    }
    return vs;
}

std::vector<glm::vec3> Geometry2D::normals(Axis axis) const
{
    switch (axis) {
    case Axis::X :
        return std::vector<glm::vec3>(vertices_2d_.size(), {1.0f, 0.0f, 0.0f});
    case Axis::Y :
        return std::vector<glm::vec3>(vertices_2d_.size(), {0.0f, 1.0f, 0.0f});
    case Axis::Z :
        return std::vector<glm::vec3>(vertices_2d_.size(), {0.0f, 0.0f, 1.0f});
    }
}

std::vector<glm::vec2> Geometry2D::texture_uvs(float scale) const
{
    std::vector<glm::vec2> uvs;
    for (glm::vec2 v2d : vertices_2d_) {
        uvs.emplace_back(v2d * scale - 0.5f);
    }
    return uvs;
}

std::vector<glm::vec2>& Geometry2D::vertices_2d()
{
    return vertices_2d_;
}

std::vector<glm::uvec3>& Geometry2D::indices()
{
    return indices_;
}

const std::vector<glm::vec2>& Geometry2D::vertices_2d() const
{
    return vertices_2d_;
}

const std::vector<glm::uvec3>& Geometry2D::indices() const
{
    return indices_;
}

Rectangle::Rectangle(float rx, float ry)
{
    vertices_2d_ = {
        {-rx,  ry},
        {-rx, -ry},
        { rx, -ry},
        { rx,  ry},
    };
    indices_ = {
        {0, 1, 2},
        {0, 2, 3},
    };
}

} // namespace geo
} // namespace mixi
