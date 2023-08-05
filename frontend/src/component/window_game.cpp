#include "component/window_game.h"
#include "mixi/engine/opengl/model.h"
#include "util/file_reader.h"
#include "game/room.h"

namespace mixi {
namespace gobang {

WindowGame::WindowGame(
    gui::Context& context,
    const ReadFirstBuffer<game::Room>& room
) :
    gui::Window(context, gettext("Gobang Game")),
    room_(room),
    datum_view_pos_(0.0f, 2.0f, 1.0f),
    camera_(std::make_shared<geo::Camera>(datum_view_pos_, glm::vec3(0.0f, 1.0f, -2.25f))),
    program_chessboard_(std::make_shared<ChessboardProgram>(
        camera_,
        gl::Shader("resource/glsl/chessboard_default.vert", GL_VERTEX_SHADER),
        gl::Shader("resource/glsl/chessboard_default.frag", GL_FRAGMENT_SHADER)
    )),
    program_chesspiece_(std::make_shared<ChesspieceProgram>(
        camera_,
        gl::Shader("resource/glsl/chesspiece_default.vert", GL_VERTEX_SHADER),
        gl::Shader("resource/glsl/chesspiece_default.frag", GL_FRAGMENT_SHADER)
    )),
    role_(game::SPACE),
    on_stone_([](std::byte){})
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    gl::eng::Model chessboard_model(
        "resource/model/chessboard.obj",
        aiProcess_FlipUVs | aiProcess_CalcTangentSpace
    );
    node_helper_(chessboard_model.root_node);
    gl::eng::Model chesspiece_model(
        "resource/model/chesspiece.obj",
        aiProcess_FlipUVs
    );
    node_helper_(chesspiece_model.root_node);

    read_stone_coors_helper_("resource/model/chessboard.obj");
}

void WindowGame::role(std::byte role)
{
    role_ = role;
}

void WindowGame::on_stone(const std::function<void(std::byte)>& f)
{
    on_stone_ = f;
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
        projection_ = glm::perspective(
            glm::radians(45.0f),
            width / height,
            0.1f, 1000.0f
        );
        frame_buffer_.resize(width, height);
        program_chessboard_->set_projection(projection_);
        program_chesspiece_->set_projection(projection_);
    }

    ImGui::Image(
        (void*)(intptr_t)frame_buffer_.texture().id(),
        content_region_size,
        ImVec2(0, 1), ImVec2(1, 0)
    );

    /* render frame */
    gl::Bind bind(frame_buffer_);
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.0f, 0.2f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* render chessboard */
    program_chessboard_->prepare_use();
    vao_chessboard_->draw();

    /* render chesspiece */
    program_chesspiece_->prepare_use();
    program_chesspiece_->set_color(BlackColor_);
    RfbReader<game::Room> room(room_);
    for (int i = 0; i < room->records.size(); i += 2) {
        std::byte coor = room->records[i];
        int r = (int)(coor >> 4) & 0x0F;
        int c = (int)coor & 0x0F;
        program_chesspiece_->set_model(
            glm::translate(glm::mat4(1.0f), stone_coors_[r][c])
        );
        vao_chesspiece_->draw();
    }
    program_chesspiece_->set_color(WhiteColor_);
    for (int i = 1; i < room->records.size(); i += 2) {
        std::byte coor = room->records[i];
        int r = (int)(coor >> 4) & 0x0F;
        int c = (int)coor & 0x0F;
        program_chesspiece_->set_model(
            glm::translate(glm::mat4(1.0f), stone_coors_[r][c])
        );
        vao_chesspiece_->draw();
    }

    /* mouse hovered */
    if (!ImGui::IsItemHovered()) {
        return;
    }
    ImGuiIO& io = ImGui::GetIO();

    /* camera view */
    ImVec2 cursor_in_frame = read_cursor_in_frame_();

    float ndc_x =  2.0f * cursor_in_frame.x / width  - 1.0f;
    float ndc_y = -2.0f * cursor_in_frame.y / height + 1.0f;

    camera_->yaw  (-ndc_x * 5.0f - 90.0f);
    camera_->pitch(-ndc_y * 5.0f);

    float unit_length = glm::length(datum_view_pos_) * 2.0f / 50.0f;
    float units = glm::length(camera_->position()) / unit_length + io.MouseWheel;
    if (units > 50.0f) {
        units = 50.0f;
    }
    if (units < 10.0f) {
        units = 10.0f;
    }

    const glm::vec3 normal_datum_view_pos = glm::normalize(datum_view_pos_);
    glm::vec3 view_pos = glm::mat4(glm::mat3(
        glm::vec3(1.0, 0.0f, 0.0f),
        normal_datum_view_pos,
        glm::cross(glm::vec3(1.0, 0.0f, 0.0f), normal_datum_view_pos)
    )) * glm::eulerAngleZX(
        glm::radians(camera_->yaw() + 90.0f),
        glm::radians(camera_->pitch())
    ) * glm::vec4(0.0f, units * unit_length, 0.0f, 1.0f);

    camera_->position(view_pos);

    /* render chesspiece pick up */
    if (role_ == game::SPACE) {
        return;
    }

    glm::vec3 cursor_in_world_coor = read_cursor_world_coor_(
        cursor_in_frame.x, cursor_in_frame.y,
        width, height
    );

    int pickup_i = -1;
    int pickup_j = -1;
    glm::vec3 piece_coor;
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {
            if (room->board[i][j] != game::SPACE) {
                continue;
            }
            if (glm::length(cursor_in_world_coor - stone_coors_[i][j]) < pickup_radius_) {
                pickup_i = i;
                pickup_j = j;
                break;
            }
        }
    }

    if (pickup_i != -1) {
        program_chesspiece_->prepare_use();
        const glm::vec3& color = role_ == game::BLACK ? BlackColor_ : WhiteColor_;
        program_chesspiece_->set_color(color);
        program_chesspiece_->set_model(
            glm::translate(glm::mat4(1.0f), stone_coors_[pickup_i][pickup_j])
        );
        vao_chesspiece_->draw();
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            on_stone_((std::byte)((pickup_i << 4) | pickup_j));
        }
    }

}

ImVec2 WindowGame::read_cursor_in_frame_()
{
    ImVec2 mouse_delta = ImGui::GetMouseDragDelta();
    ImVec2 mouse_pos  = ImGui::GetMousePos();
    ImVec2 window_pos = ImGui::GetWindowPos();
    ImVec2 image_pos  = ImGui::GetWindowContentRegionMin();
    image_pos.x += window_pos.x;
    image_pos.y += window_pos.y;
    return ImVec2(mouse_pos.x - image_pos.x, mouse_pos.y - image_pos.y);
}

glm::vec3 WindowGame::read_cursor_world_coor_(
    float x, float y,
    float width, float height
) {
    float depth;
    glReadPixels(
        x, height - y,
        1, 1,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        &depth
    );

    glm::vec4 clip(
        (2.0f * x / width) - 1.0f,
        1.0f - (2.0f * y / height),
        2.0f * depth - 1.0f,
        1.0f
    );

    glm::vec4 world_coords = glm::inverse(projection_ * camera_->view_matrix()) * clip;
    return world_coords / world_coords.w;
}

void WindowGame::node_helper_(const gl::eng::Node& node) {
    std::cout << node.name << std::endl;
    if (node.name == "chessboard") {
        if (node.meshes.empty()) {
            throw std::runtime_error("wrong chessboard model");
        }
        program_chessboard_->set_material(node.meshes[0]->material.get());
        vao_chessboard_ = program_chessboard_->gen_vertex_array(node.meshes[0]->vertex_buffer);
    }
    if (node.name == "chesspiece") {
        vao_chesspiece_ = program_chesspiece_->gen_vertex_array(node.meshes[0]->vertex_buffer);
    }
    for (const gl::eng::Node& child : node.children) {
        node_helper_(child);
    }
}

void WindowGame::read_stone_coors_helper_(const std::filesystem::path& filepath)
{
    char nouse;
    float x, y, z;
    glm::vec3 lower_left, lower_right, upper_left;
    int status = 0b000;
    std::string coor_string;
    std::string obj_name;
    FileReverseReader reader(filepath);
    while (status != 0b111 && reader.getline(obj_name)) {
        if (obj_name == "o lower-left") {
            std::stringstream ss(coor_string);
            ss >> nouse >> x >> y >> z;
            lower_left = glm::vec3(x, y, z);
            status |= 0b001;
        }
        else if (obj_name == "o lower-right") {
            std::stringstream ss(coor_string);
            ss >> nouse >> x >> y >> z;
            lower_right = glm::vec3(x, y, z);
            status |= 0b010;
        }
        else if (obj_name == "o upper-left") {
            std::stringstream ss(coor_string);
            ss >> nouse >> x >> y >> z;
            upper_left = glm::vec3(x, y, z);
            status |= 0b100;
        }
        coor_string = std::move(obj_name);
    }
    glm::vec3 vx = (lower_right - lower_left) / 14;
    glm::vec3 vy = (upper_left  - lower_left) / 14;
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {
            stone_coors_[i][j] = lower_left + vy * i + vx * j;
        }
    }
    pickup_radius_ = glm::length(vx) * 0.5;
}

const glm::vec3 WindowGame::BlackColor_ = glm::vec3(0.10f, 0.10f, 0.10f);
const glm::vec3 WindowGame::WhiteColor_ = glm::vec3(0.90f, 0.90f, 0.90f);

} // namespace gobang
} // namespace mixi
