#include "engine/opengl/drawable_node.h"

namespace mixi
{
namespace gl
{
namespace eng
{

DrawableNode::DrawableNode(
    const ModelProgram& p,
    const VertexArray& v,
    const glm::mat4& m
) : 
    model_program_(p),
    vertex_array_(v),
    model_matrix_(m)
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

const glm::mat4& DrawableNode::model_matrix() const
{
    return model_matrix_;
}

void DrawableNode::model_matrix(const glm::mat4& m)
{
    model_matrix_ = m;
}

void DrawableNode::draw(const glm::mat4& pre_model) const
{
    glm::mat4 model = model_matrix_ * pre_model;
    model_program_.update_model(glm::value_ptr(model));
    vertex_array_.draw();
}

DrawableGroup::DrawableGroup(const glm::mat4& m) :
    model_matrix_(m)
{

}

std::vector<DrawableNode>& DrawableGroup::node_members()
{
    return node_members_;
}

std::vector<DrawableGroup>& DrawableGroup::group_members()
{
    return group_members_;
}

void DrawableGroup::draw(const glm::mat4& pre_model) const
{
    glm::mat4 model = model_matrix_ * pre_model;
    for (const DrawableNode& node : node_members_) {
        node.draw(model);
    }
    for (const DrawableGroup& group : group_members_) {
        group.draw(model);
    }
}

} // namespace eng
} // namespace gl
} // namespace mixi
