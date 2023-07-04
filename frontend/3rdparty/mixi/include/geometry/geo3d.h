#pragma once
#include "pch.h"
#include "geometry/geo2d.h"

namespace mixi
{
namespace geo
{

class Geometry3D
{

public:

    virtual std::vector<glm::vec3> normals();
    virtual std::vector<glm::vec2> texture_uvs();

    std::vector<glm::vec3>& vertices();
    std::vector<glm::uvec3>& indices();

protected:

    std::vector<glm::vec3> vertices_;
    std::vector<glm::uvec3> indices_;

};

class Stretch : public Geometry3D
{

public:

    Stretch(
        const Geometry2D& bottom,
        float h,
        Axis axis = Axis::Z,
        bool smooth = false
    );

private:

    void construct_smooth_(
        const Geometry2D& bottom,
        float h,
        Axis axis
    );

    void construct_(
        const Geometry2D& bottom,
        float h,
        Axis axis
    );

};

} // namespace geo
} // namespace mixi
