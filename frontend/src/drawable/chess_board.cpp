/*
#include "drawable/chess_board.h"
#include "geometry/geo3d.h"

namespace mixi
{
namespace gobang
{

ChessBoard::ChessBoard(
    const gl::eng::ModelProgram& program,
    const glm::mat4& m
) :
    gl::eng::DrawableGroup(m)
{
    geo::Rectangle rectangle(5.0f, 5.0f);

    gl::VertexBuffer::Ptr vbo_top_ = std::make_shared<gl::VertexBuffer>(
        rectangle.vertices().size() * sizeof(glm::vec3),
        (void*)rectangle.vertices().data(),
        gl::VertexBuffer::Usage::STATIC_DRAW,
        rectangle.vertices().size(),
        std::vector<gl::VertexBuffer::Descriptor>{{
            .size       = 3,
            .type       = GL_FLOAT,
            .normalized = GL_FALSE,
            .stride     = sizeof(glm::vec3),
            .pointer    = (void*)0
        }}
    );
    vao_top_.bind_vertex_buffer(vbo_top_, {{0, 0}});

    gl::ElementBuffer::Ptr ebo_top_ = std::make_shared<gl::ElementBuffer>(
        rectangle.indices().size() * sizeof(glm::uvec3),
        (void*)rectangle.indices().data()
    );
    vao_top_.bind_element_buffer(ebo_top_);

    geo::Stretch stretch(rectangle, 0.5f);
    gl::VertexBuffer::Ptr vbo_side_ = std::make_shared<gl::VertexBuffer>(
        stretch.vertices().size() * sizeof(glm::vec3),
        (void*)stretch.vertices().data(),
        gl::VertexBuffer::Usage::STATIC_DRAW,
        stretch.vertices().size(),
        std::vector<gl::VertexBuffer::Descriptor>{{
            .size       = 3,
            .type       = GL_FLOAT,
            .normalized = GL_FALSE,
            .stride     = sizeof(glm::vec3),
            .pointer    = (void*)0
        }}
    );
    vao_side_.bind_vertex_buffer(vbo_side_, {{0, 0}});

    gl::ElementBuffer::Ptr ebo_side_ = std::make_shared<gl::ElementBuffer>(
        stretch.indices().size() * sizeof(glm::uvec3),
        (void*)stretch.indices().data()
    );
    vao_side_.bind_element_buffer(ebo_side_);

    // node_members_.emplace_back(
    //     std::make_shared<gl::eng::DrawableNode>(program, vao_top_)
    // );
    node_members_.emplace_back(
        std::make_shared<gl::eng::DrawableNode>(program, vao_side_)
    );
    // node_members_[0]->translate({0.0f, 0.0f, 0.5f});
}

} // namespace gobang
} // namespace mixi

*/
