#include "mixi/geometry/geo3d.h"

namespace mixi
{
namespace geo
{

std::vector<glm::vec3> Geometry3D::normals()
{
    std::vector<glm::vec3> vs(vertices_.size());
    for (glm::uvec3 index : indices_) {
        glm::vec3 a = vertices_[index.y] - vertices_[index.x];
        glm::vec3 b = vertices_[index.z] - vertices_[index.x];
        glm::vec3 norm = glm::normalize(glm::cross(a, b));
        vs[index.x] += norm;
        vs[index.y] += norm;
        vs[index.z] += norm;
    }
    for (glm::vec3& norm : vs) {
        norm = glm::normalize(norm);
    }
    return vs;
}

std::vector<glm::vec2> Geometry3D::texture_uvs()
{
    return std::vector<glm::vec2>(vertices_.size());
}

std::vector<glm::vec3>& Geometry3D::vertices()
{
    return vertices_;
}

std::vector<glm::uvec3>& Geometry3D::indices()
{
    return indices_;
}

Stretch::Stretch(
    const Geometry2D& bottom,
    float h,
    Axis axis,
    bool smooth
) {
    if (smooth) {
        construct_smooth_(bottom, h, axis);
    } else {
        construct_(bottom, h, axis);
    }
}

void Stretch::construct_smooth_(
    const Geometry2D& bottom,
    float h,
    Axis axis
) {
    const std::vector<glm::vec2>& vs2d = bottom.vertices_2d();
    const int n = vs2d.size();
    const int m = n * 2;
    vertices_.reserve(m);
    indices_.reserve(m);
    for (int i = 0; i < n; i++) {
        glm::vec2 uv = vs2d[i];
        switch (axis) {
        case Axis::X :
            vertices_.emplace_back(0.0f, uv);
            vertices_.emplace_back(h,    uv);
            break;
        case Axis::Y :
            vertices_.emplace_back(uv.x, 0.0f, uv.y);
            vertices_.emplace_back(uv.x, h,    uv.y);
            break;
        case Axis::Z :
            vertices_.emplace_back(uv, 0.0f);
            vertices_.emplace_back(uv, h);
            break;
        }
        const int j = i * 2;
        indices_.emplace_back(j,     (j + 2) % m,  j + 1);
        indices_.emplace_back(j + 1, (j + 2) % m, (j + 3) % m);
    }
}

void Stretch::construct_(
    const Geometry2D& bottom,
    float h,
    Axis axis
) {
    const std::vector<glm::vec2>& vs2d = bottom.vertices_2d();
    const int n = vs2d.size();
    const int m = n * 4;
    vertices_.reserve(m);
    indices_.reserve(n * 2);
    for (int i = 0; i < n; i++) {
        glm::vec2 uv = bottom.vertices_2d()[i];
        switch (axis) {
        case Axis::X :
            vertices_.emplace_back(0.0f, uv);
            vertices_.emplace_back(h,    uv);
            break;
        case Axis::Y :
            vertices_.emplace_back(uv.x, 0.0f, uv.y);
            vertices_.emplace_back(uv.x, h,    uv.y);
            break;
        case Axis::Z :
            vertices_.emplace_back(uv, 0.0f);
            vertices_.emplace_back(uv, h);
            break;
        }
        const int j = i * 4;
        if (i >= 1) {
            vertices_.push_back(vertices_[j - 2]);
            vertices_.push_back(vertices_[j - 1]);
        }
        indices_.emplace_back(j,     (j + 2) % m,  j + 1);
        indices_.emplace_back(j + 1, (j + 2) % m, (j + 3) % m);
    }
    vertices_.push_back(vertices_[0]);
    vertices_.push_back(vertices_[1]);
}

} // namespace geo
} // namespace mixi
