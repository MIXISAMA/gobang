#include "component/window_game.h"


namespace mixi {
namespace gobang {

WindowGame::WindowGame(imgui::Context& context) :
    imgui::Window(context, gettext("Gobang Game")),
    camera_({0.0f, 0.0f, 10.0f}),
    plane_(),
    vertex_buffer_(
        plane_.vertices().size() * sizeof(glm::vec3),
        (void*)plane_.vertices().data(),
        GL_STATIC_DRAW,
        plane_.vertices().size(),
        {gl::VertexBuffer::Descriptor{
            .size       = (GLint)plane_.vertices().size(),
            .type       = GL_FLOAT,
            .normalized = GL_FALSE,
            .stride     = 3 * sizeof(float),
            .pointer    = (void*)0
        }}
    ),
    vert_shader_("resource/glsl/demo.vert", GL_VERTEX_SHADER),
    frag_shader_("resource/glsl/demo.frag", GL_FRAGMENT_SHADER),
    program_(vert_shader_, frag_shader_),
    projection_(glm::perspective(
        glm::radians(45.0f),
        (float)frame_buffer_.texture().width() /
        (float)frame_buffer_.texture().height(),
        1.0f, 10000.0f
    )),
    view_(camera_.view_matrix()),
    model_(1.0f)
{
    vertex_array_.bind_vertex_buffer(vertex_buffer_, {{0, 0}});
    GLint location_view_       = program_.get_uniform_location("view");
    GLint location_model_      = program_.get_uniform_location("model");
    GLint location_projection_ = program_.get_uniform_location("projection");
    gl::Bind b(frame_buffer_.texture());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

WindowGame::~WindowGame()
{
    // delete frame_buffer_;
}

void WindowGame::content()
{
    /* on resize window */
    ImVec2 content_region_size = ImGui::GetContentRegionAvail();
    float width  = content_region_size.x;
    float height = content_region_size.y;
    if (width == 0 || height == 0) {
        return;
    }
    if (
        width  != frame_buffer_.texture().width() ||
        height != frame_buffer_.texture().height()
    ) {
        frame_buffer_.resize(width, height);
        projection_ = glm::perspective(
            glm::radians(45.0f),
            (float)width / (float)height,
            1.0f, 10000.0f
        );
    }

    /* render frame */
    frame_buffer_.bind();
    
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.0f, 0.2f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program_.use();
    program_.set_uniform_mat4(location_view_,       glm::value_ptr(view_));
    program_.set_uniform_mat4(location_model_,      glm::value_ptr(model_));
    program_.set_uniform_mat4(location_projection_, glm::value_ptr(projection_));
    vertex_array_.draw();

    frame_buffer_.unbind();

    ImGui::Image(
        (void*)(intptr_t)frame_buffer_.texture().id(),
        content_region_size
    );

    /* calculate transformation */
    if (!ImGui::IsItemHovered()) {
        return;
    }
    ImGuiIO& io = ImGui::GetIO();
    if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        camera_.yaw_right_pitch_up_(
            io.MouseDelta.x * 0.3f,
            io.MouseDelta.y * 0.3f
        );
    }
    if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
        camera_.move_right(io.MouseDelta.x * 0.01f);
        camera_.move_up   (io.MouseDelta.y * 0.01f);
    }
    camera_.move_forward(io.MouseWheel);
    view_ = camera_.view_matrix();
}

void WindowGame::resize_frame_buffer_()
{
    
}

} // namespace gobang
} // namespace mixi