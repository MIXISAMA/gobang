#pragma once
#include "pch.h"

namespace mixi
{
namespace geo
{

class Vertices
{

public:

    const std::vector<glm::vec3>& vertices() const;

protected:

    std::vector<glm::vec3> vertices_;

};

class Indices
{

public:

    const std::vector<glm::uvec3>& indices() const;

protected:

    std::vector<glm::uvec3> indices_;

};

} // namespace geo
} // namespace mixi
