#include "component/window_game.h"
#include "mixi/engine/opengl/model.h"

namespace mixi {
namespace gobang {



WindowGame::WindowGame(gui::Context& context) :
    gui::Window(context, gettext("Gobang Game")),
    camera_({0.0f, 0.0f, 20.0f}),
    uniform_buffer_camera_(new gl::eng::CameraUniformBuffer()),
    program_(new gl::eng::ModelCameraProgram(
        uniform_buffer_camera_, 0,
        gl::Shader("resource/glsl/multiple_lights.vert", GL_VERTEX_SHADER),
        gl::Shader("resource/glsl/demo2.frag", GL_FRAGMENT_SHADER)
        // gl::Shader("resource/glsl/demo.vert", GL_VERTEX_SHADER),
        // gl::Shader("resource/glsl/demo2.frag", GL_FRAGMENT_SHADER)
    )),
    chessboard_model_("resource/model/chessboard.obj")
{
    glm::mat4 model(1.0f);
    program_->update_model(glm::value_ptr(model));
    node_helper_(chessboard_model_.root_node);
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
        uniform_buffer_camera_->update_projection(
            glm::value_ptr(projection)
        );
    }

    // drawable_group_.rotate(0.5f, {1, 1, 1});

    /* render frame */
    frame_buffer_.bind();
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.0f, 0.2f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program_->use();
    chessboard_model_.draw();

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
    uniform_buffer_camera_->update_view(glm::value_ptr(view));
}

void WindowGame::node_helper_(const gl::eng::Node& node) {
    std::cout << node.name << std::endl;
    if (node.name == "chessboard") {
        chessboard_node_ = &node;
    }
    for (const gl::eng::Node& child : node.children) {
        node_helper_(child);
    }
}

} // namespace gobang
} // namespace mixi