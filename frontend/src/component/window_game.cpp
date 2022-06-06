#include "component/window_game.h"
#include "geometry/plane.h"

namespace mixi {
namespace gobang {

WindowGame::WindowGame(imgui::Context& context) :
    imgui::Window(context, gettext("Gobang Game")),
    camera_({0.0f, 0.0f, 20.0f}),
    program_(
        "model", uniform_buffer_,
        gl::Shader("resource/glsl/demo.vert", GL_VERTEX_SHADER),
        gl::Shader("resource/glsl/demo.frag", GL_FRAGMENT_SHADER)
    )
{
    geo::Plane plane;

    vertex_buffer_ = new gl::VertexBuffer(
        plane.vertices().size() * sizeof(glm::vec3),
        (void*)plane.vertices().data(),
        GL_STATIC_DRAW,
        plane.vertices().size(),
        {gl::VertexBuffer::Descriptor{
            .size       = 3,
            .type       = GL_FLOAT,
            .normalized = GL_FALSE,
            .stride     = sizeof(glm::vec3),
            .pointer    = (void*)0
        }}
    );

    element_buffer_ = new gl::ElementBuffer(
        plane.indices().size() * sizeof(glm::uvec3),
        (void*)plane.indices().data()
    );

    vertex_array_.bind_vertex_buffer(*vertex_buffer_, {{0, 0}});
    vertex_array_.bind_element_buffer(*element_buffer_);

    drawable_group_.node_members().emplace_back(
        program_, vertex_array_
    );
}

WindowGame::~WindowGame()
{
    delete vertex_buffer_;
    delete element_buffer_;
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
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            width / height,
            0.1f, 1000.0f
        );
        uniform_buffer_.update_projection(
            glm::value_ptr(projection)
        );
    }

    /* render frame */
    frame_buffer_.bind();
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.0f, 0.2f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawable_group_.draw();
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
    glm::mat4 view = camera_.view_matrix();
    uniform_buffer_.update_view(glm::value_ptr(view));
}

} // namespace gobang
} // namespace mixi