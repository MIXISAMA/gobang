/*
#include "engine/opengl/drawable_node.h"

namespace mixi
{
namespace gl
{
namespace eng
{

ModelMatrix::ModelMatrix(const glm::mat4& m) :
    model_matrix_(m)
{

}

const glm::mat4& ModelMatrix::model_matrix() const
{
    return model_matrix_;
}

void ModelMatrix::model_matrix(const glm::mat4& m)
{
    model_matrix_ = m;
}

void ModelMatrix::translate(const glm::vec3& v)
{
    model_matrix_ = glm::translate(model_matrix_, v);
}

void ModelMatrix::scale(const glm::vec3& v)
{
    model_matrix_ = glm::scale(model_matrix_, v);
}

void ModelMatrix::rotate(float degree, const glm::vec3& axis)
{
    model_matrix_ = glm::rotate(model_matrix_, glm::radians(degree), axis);
}

DrawableNode::DrawableNode(
    const ModelProgram& p,
    const VertexArray& v,
    const glm::mat4& m
) : 
    model_program_(p),
    vertex_array_(v),
    ModelMatrix(m)
{

}

const ModelProgram& DrawableNode::model_program() const
{
    return model_program_;
}

const VertexArray& DrawableNode::vertex_array() const
{
    return vertex_array_;
}

void DrawableNode::draw(const glm::mat4& pre_model) const
{
    glm::mat4 model = model_matrix_ * pre_model;
    model_program_.update_model(glm::value_ptr(model));
    vertex_array_.draw();
}

DrawableGroup::DrawableGroup(const glm::mat4& m) :
    ModelMatrix(m)
{

}

std::vector<DrawableNode::Ptr>& DrawableGroup::node_members()
{
    return node_members_;
}

std::vector<DrawableGroup::Ptr>& DrawableGroup::group_members()
{
    return group_members_;
}

void DrawableGroup::draw(const glm::mat4& pre_model) const
{
    glm::mat4 model = model_matrix_ * pre_model;
    for (const DrawableNode::Ptr& node : node_members_) {
        node->draw(model);
    }
    for (const DrawableGroup::Ptr& group : group_members_) {
        group->draw(model);
    }
}

} // namespace eng
} // namespace gl
} // namespace mixi
*/