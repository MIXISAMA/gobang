#pragma once
#include "pch.h"

namespace mixi
{
namespace gl
{
namespace eng
{

class Pose
{
public:

    Pose(const glm::mat4& m = glm::mat4(1.0f));

    const glm::mat4& matrix() const;
    void matrix(const glm::mat4& m);

    void translate(const glm::vec3& v);
    void scale(const glm::vec3& v);
    void rotate(float degree, const glm::vec3& axis);

protected:

    glm::mat4 matrix_;
};

class PoseNode : public Pose
{

public:

    using Ptr = std::shared_ptr<PoseNode>;

    std::list<Ptr>& downstream_nodes();

protected:

    std::list<Ptr> downstream_nodes_;

};



} // namespace eng
} // namespace gl
} // namespace mixi
