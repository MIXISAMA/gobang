#include "component/window_game.h"
#include "mixi/engine/opengl/model.h"

namespace mixi {
namespace gobang {

WindowGame::WindowGame(gui::Context& context) :
    gui::Window(context, gettext("Gobang Game")),
    camera_({0.0f, 0.0f, 20.0f}),
    uniform_buffer_camera_(new gl::eng::CameraUniformBuffer()),
    program_(new gl::eng::ModelCameraProgram(
        uniform_buffer_camera_,
        gl::Shader("resource/glsl/multiple_lights.vert", GL_VERTEX_SHADER),
        gl::Shader("resource/glsl/demo2.frag", GL_FRAGMENT_SHADER)
        // gl::Shader("resource/glsl/demo.vert", GL_VERTEX_SHADER),
        // gl::Shader("resource/glsl/demo2.frag", GL_FRAGMENT_SHADER)
    ))
{
    // geo::Plane plane;

    // gl::VertexBuffer::Ptr vertex_buffer_ = std::make_shared<gl::VertexBuffer>(
    //     plane.vertices().size() * sizeof(glm::vec3),
    //     (void*)plane.vertices().data(),
    //     gl::VertexBuffer::Usage::STATIC_DRAW,
    //     plane.vertices().size(),
    //     std::vector<gl::VertexBuffer::Descriptor>{{
    //         .size       = 3,
    //         .type       = GL_FLOAT,
    //         .normalized = GL_FALSE,
    //         .stride     = sizeof(glm::vec3),
    //         .pointer    = (void*)0
    //     }}
    // );

    // gl::ElementBuffer::Ptr element_buffer_ = std::make_shared<gl::ElementBuffer>(
    //     plane.indices().size() * sizeof(glm::uvec3),
    //     (void*)plane.indices().data()
    // );

    // vertex_array_.bind_vertex_buffer(vertex_buffer_, {{0, 0}});
    // vertex_array_.bind_element_buffer(element_buffer_);

    // drawable_group_.node_members().emplace_back(program_, vertex_array_);
    // drawable_group_.node_members().emplace_back(program_, vertex_array_);
    // drawable_group_.node_members()[1].rotate(45.0f, {0,0,1});

    // drawable_group_.group_members().emplace_back(
    //     std::make_shared<ChessBoard>(program_)
    // );

    // Assimp::Importer importer;
    // const aiScene* scene = importer.ReadFile(
    //     "/Users/zhangjunbo/blender-workspace/chessboard_demo.obj",
    //     aiProcess_Triangulate | aiProcess_FlipUVs
    // );
    
    // for (int i = 0; i < scene->mNumMaterials; i++) {
    //     const aiMaterial* material = scene->mMaterials[i];
    //     for (int j = 0; j < material->mNumProperties; j++) {
    //         const aiMaterialProperty* property = material->mProperties[j];
    //         std::cout << i << '-' << j << ": " 
    //             << property->mKey.C_Str() << "\t\t";
    //         if (property->mDataLength / 4 > 3) {
    //             std::cout << (char*)property->mData;
    //         }
    //         else for (int k = 0; k < property->mDataLength / 4; k++) {
    //             std::cout << ((float*)property->mData)[k] << ", ";
    //         }
    //         std::cout << std::endl;
    //     }
    //     aiColor3D color (0.f,0.f,0.f);
    //     material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    //     std::cout << i << "-AI_MATKEY_COLOR_DIFFUSE:\t\t" << color.r << ", " << color.g << ", " << color.b << std::endl;
    // }
    
    // const aiNode* root_node = scene->mRootNode;
    // for (int i = 0; i < root_node->mNumMeshes; i++) {
    //     root_node->mMeshes[i];
    // }

    // gl::eng::ModelCreator creator;

    // gl::eng::Model* model = creator.new_model(
    //     "/Users/zhangjunbo/blender-workspace/chessboard.obj"
    // );


    // int i = 0;
    // i++;
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
    // drawable_group_.draw();
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

} // namespace gobang
} // namespace mixi