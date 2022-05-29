#pragma once
#include "pch.h"

namespace mixi
{
namespace geo
{

class Plane
{

public:

    Plane();

    const std::vector<glm::vec3>& vertices();
    const std::vector<glm::ivec3>& indices();

private:

    std::vector<glm::vec3> vertices_;
    std::vector<glm::ivec3> indices_;

};

} // namespace geo
} // namespace mixi
