#pragma once
#include "pch.h"

#include "engine/opengl/program.h"
#include "render/opengl/vertex_array.h"

namespace mixi
{
namespace gl
{
namespace eng
{

class ModelMatrix
{

public:

    ModelMatrix(const glm::mat4& m = glm::mat4(1.0f));

    const glm::mat4& model_matrix() const;
    void model_matrix(const glm::mat4& m);

    void translate(const glm::vec3& v);
    void scale(const glm::vec3& v);
    void rotate(float degree, const glm::vec3& axis);

protected:

    glm::mat4 model_matrix_;

};

class DrawableNode : public ModelMatrix
{

public:

    using Ptr = std::shared_ptr<DrawableNode>;

    DrawableNode(
        const ModelProgram& p,
        const VertexArray& v,
        const glm::mat4& m = glm::mat4(1.0f)
    );

    const ModelProgram& model_program() const;
    const VertexArray& vertex_array() const;

    void draw(const glm::mat4& pre_model = glm::mat4(1.0f)) const;

protected:

    const ModelProgram& model_program_;
    const VertexArray& vertex_array_;

};

class DrawableGroup : public ModelMatrix
{

public:

    using Ptr = std::shared_ptr<DrawableGroup>;

    DrawableGroup(const glm::mat4& m = glm::mat4(1.0f));

    std::vector<DrawableNode::Ptr>&  node_members();
    std::vector<DrawableGroup::Ptr>& group_members();

    void draw(const glm::mat4& pre_model = glm::mat4(1.0f)) const;

protected:

    std::vector<DrawableNode::Ptr>  node_members_;
    std::vector<DrawableGroup::Ptr> group_members_;

};


class ModelingTransformation
{
public:

    ModelingTransformation(const glm::mat4& m = glm::mat4(1.0f));

    const glm::mat4& matrix() const;
    void matrix(const glm::mat4& m);

    void translate(const glm::vec3& v);
    void scale(const glm::vec3& v);
    void rotate(float degree, const glm::vec3& axis);

protected:

    glm::mat4 matrix_;
};

class ModelingTransformationNode
{

};

class ModelingTransformationNodeGroup
{

};

} // namespace eng
} // namespace gl
} // namespace mixi
