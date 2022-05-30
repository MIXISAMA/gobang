#include "component/window_game.h"


namespace mixi {
namespace gobang {

WindowGame::WindowGame(imgui::Context& context) :
    imgui::Window(context, gettext("Gobang Game")),
    camera_({0.0f, 0.0f, 20.0f}),
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
    vertex_array_(GL_POINTS),
    program_(
        "model", uniform_buffer_,
        gl::Shader("resource/glsl/demo.vert", GL_VERTEX_SHADER),
        gl::Shader("resource/glsl/demo.frag", GL_FRAGMENT_SHADER)
    )
{
    vertex_array_.bind_vertex_buffer(vertex_buffer_, {{0, 0}});

    gl::Bind b(frame_buffer_.texture());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    drawable_group_.node_members().emplace_back(
        program_, vertex_array_
    );
}

WindowGame::~WindowGame()
{

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