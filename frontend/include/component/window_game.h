#pragma once
#include "pch.h"
#include "gui/imgui/component/window.h"

#include "geometry/camera.h"
#include "geometry/plane.h"

#include "render/opengl/buffer.h"
#include "render/opengl/vertex_array.h"

#include "engine/opengl/program/camera_model.h"
#include "engine/opengl/uniform_buffer/camera.h"
#include "engine/opengl/drawable_node.h"

namespace mixi
{
namespace gobang
{

class WindowGame : public imgui::Window
{
public:

    WindowGame(imgui::Context& context);
    ~WindowGame();

    void content() override;

private:

    geo::Camera camera_;
    geo::Plane plane_;

    gl::FrameBuffer frame_buffer_;
    gl::VertexBuffer vertex_buffer_;
    // gl::ElementBuffer element_buffer_;
    gl::VertexArray vertex_array_;

    gl::eng::CameraUniformBuffer uniform_buffer_;
    gl::eng::CameraModelProgram program_;
    gl::eng::DrawableGroup drawable_group_;

};

} // namespace gobang
} // namespace mixi
