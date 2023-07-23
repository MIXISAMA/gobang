#pragma once
#include "pch.h"
#include "mixi/gui/component.h"
#include "mixi/geometry/camera.h"
#include "mixi/render/opengl/buffer.h"
#include "mixi/engine/opengl/program.h"
#include "mixi/engine/opengl/model.h"


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

    geo::Camera camera_;

    gl::FrameBuffer frame_buffer_;

    gl::eng::CameraUniformBuffer::Ptr uniform_buffer_camera_;
    gl::eng::ModelCameraProgram::Ptr program_;
    // gl::eng::DrawableGroup drawable_group_;

    gl::eng::Model chessboard_model_;
    const gl::eng::Node* chessboard_node_;

    void node_helper_(const gl::eng::Node&);

};

} // namespace gobang
} // namespace mixi
