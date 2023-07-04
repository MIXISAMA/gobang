#pragma once
#include "pch.h"
#include "gui/component.h"

#include "geometry/camera.h"

#include "render/opengl/buffer.h"
#include "render/opengl/vertex_array.h"

#include "engine/opengl/program.h"

// #include "engine/opengl/drawable_node.h"

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
    gl::VertexArray vertex_array_;

    gl::eng::CameraUniformBuffer::Ptr uniform_buffer_camera_;
    gl::eng::ModelCameraProgram::Ptr program_;
    // gl::eng::DrawableGroup drawable_group_;

};

} // namespace gobang
} // namespace mixi
