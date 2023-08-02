#pragma once
#include "pch.h"
#include "mixi/gui/component.h"
#include "mixi/geometry/camera.h"
#include "mixi/render/opengl/buffer.h"
#include "mixi/engine/opengl/model.h"
#include "drawable/chessboard.h"
#include "drawable/chesspiece.h"


namespace mixi
{
namespace gobang
{

class WindowGame : public gui::Window
{
public:

    WindowGame(gui::Context& context);
    ~WindowGame();

    void role(std::byte role);
    void on_stone(std::function<void(int, int)> f);

protected:

    void content() override;

private:

    gl::FrameBuffer frame_buffer_;

    std::shared_ptr<geo::Camera> camera_;
    std::shared_ptr<ChessboardProgram> program_chessboard_;
    std::shared_ptr<ChesspieceProgram> program_chesspiece_;
    std::shared_ptr<gl::VertexArray> vao_chessboard_;
    std::shared_ptr<gl::VertexArray> vao_chesspiece_;

    glm::mat4 projection_;

    glm::vec3 stone_coors_[15][15];
    float pickup_radius_;

    std::byte role_;
    std::function<void(int, int)> on_stone_;

    ImVec2 read_cursor_in_frame_();
    glm::vec3 read_cursor_world_coor_(
        float x, float y,
        float width, float height
    );

    void node_helper_(const gl::eng::Node&);
    void read_stone_coors_helper_(const std::filesystem::path& filepath);

};

} // namespace gobang
} // namespace mixi
