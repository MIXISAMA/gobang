#pragma once
#include "pch.h"

namespace mixi
{
namespace gl
{
namespace eng
{

struct PhongLight
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight : public PhongLight
{
    glm::vec3 position;
    float attenuation[3];
};

struct DirLight : public PhongLight
{
    glm::vec3 direction;
};

struct SpotLight : public PhongLight
{
    glm::vec3 position;
    glm::vec3 direction;
    float attenuation[3];
    float cutOff;
    float outerCutOff;
};

} // namespace eng
} // namespace gl
} // namespace mixi
