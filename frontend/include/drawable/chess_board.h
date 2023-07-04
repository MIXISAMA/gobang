#pragma once
#include "pch.h"
#include "mixi/render/opengl/vertex_array.h"
#include "mixi/engine/opengl/program.h"

// #include "engine/opengl/drawable_node.h"

namespace mixi
{
namespace gobang
{

// class ChessBoard : public gl::eng::DrawableGroup
// {

// public:

//     ChessBoard(
//         const gl::eng::ModelProgram& program,
//         const glm::mat4& m = glm::mat4(1.0f)
//     );

// protected:

//     gl::VertexArray vao_side_;
//     gl::VertexArray vao_top_;

// };

class Chessboard
{

public:

    Chessboard(
        const gl::eng::ModelCameraProgram& program,
        const glm::mat4& m = glm::mat4(1.0f)
    );

protected:

    gl::VertexArray vao_chessboard_;

};

} // namespace mixi
} // namespace gobang
