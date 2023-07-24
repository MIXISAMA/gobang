#pragma once
#include "pch.h"
#include "mixi/gui/component.h"
#include "mixi/geometry/camera.h"
#include "mixi/render/opengl/buffer.h"
#include "mixi/engine/opengl/model.h"
#include "drawable/program.h"


namespace mixi
{
namespace gobang
{

class WindowGame : public gui::Window
{
public:

    WindowGame(gui::Context& context);
    ~WindowGame();

    void content() override;

private:

    gl::FrameBuffer frame_buffer_;

    std::shared_ptr<geo::Camera> camera_;
    std::shared_ptr<ChessboardProgram> program_;
    std::shared_ptr<gl::VertexArray> vao_chessboard_;

    // gl::eng::Model chessboard_model_;
    // const gl::eng::Node* chessboard_node_;

    void node_helper_(const gl::eng::Node&);

};

} // namespace gobang
} // namespace mixi
