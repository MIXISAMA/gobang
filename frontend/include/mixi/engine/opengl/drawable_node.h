#pragma once
#include "pch.h"

#include "engine/opengl/program/model.h"
#include "render/opengl/vertex_array.h"

namespace mixi
{
namespace gl
{
namespace eng
{

class DrawableNode
{

public:

    DrawableNode(
        const ModelProgram& p,
        const VertexArray& v,
        const glm::mat4& m = glm::mat4(1.0f)
    );

    const ModelProgram& model_program() const;
    const VertexArray& vertex_array() const;

    const glm::mat4& model_matrix() const;
    void model_matrix(const glm::mat4& m);

    void draw(const glm::mat4& pre_model = glm::mat4(1.0f)) const;

protected:

    const ModelProgram& model_program_;
    const VertexArray& vertex_array_;
    glm::mat4 model_matrix_;

};

class DrawableGroup
{

public:

    DrawableGroup(const glm::mat4& m = glm::mat4(1.0f));

    std::vector<DrawableNode>&  node_members();
    std::vector<DrawableGroup>& group_members();

    void draw(const glm::mat4& pre_model = glm::mat4(1.0f)) const;

protected:

    std::vector<DrawableNode>  node_members_;
    std::vector<DrawableGroup> group_members_;

    glm::mat4 model_matrix_;

};

} // namespace eng
} // namespace gl
} // namespace mixi
