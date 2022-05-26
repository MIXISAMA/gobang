#pragma once
#include "pch.h"
#include "gui/imgui/component/window.h"

#include "geo/camera.h"
#include "geo/plane.h"

#include "render/opengl/buffer.h"
#include "render/opengl/vertex_array.h"
#include "render/opengl/program.h"

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

    gl::Shader vert_shader_;
    gl::Shader frag_shader_;
    gl::Program program_;

    glm::mat4 view_;
    glm::mat4 model_;
    glm::mat4 projection_;

    GLint location_view_;
    GLint location_model_;
    GLint location_projection_;

    void resize_frame_buffer_();

};

} // namespace gobang
} // namespace mixi
