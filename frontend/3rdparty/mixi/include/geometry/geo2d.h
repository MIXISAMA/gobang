#pragma once
#include "pch.h"

namespace mixi
{
namespace geo
{

enum class Axis
{
    X, Y, Z,
};

class Geometry2D
{

public:

    virtual std::vector<glm::vec3> vertices(Axis axis = Axis::Z) const;
    virtual std::vector<glm::vec3> normals(Axis axis = Axis::Z) const;
    virtual std::vector<glm::vec2> texture_uvs(float scale = 1.0f) const;

    std::vector<glm::vec2>& vertices_2d();
    std::vector<glm::uvec3>& indices();

    const std::vector<glm::vec2>& vertices_2d() const;
    const std::vector<glm::uvec3>& indices() const;

protected:

    std::vector<glm::vec2> vertices_2d_;
    std::vector<glm::uvec3> indices_;

};

class Rectangle : public Geometry2D
{

public:

    Rectangle(float rx, float ry);

};

} // namespace geo
} // namespace mixi
