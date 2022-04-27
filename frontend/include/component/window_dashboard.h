#pragma once
#include "pch.h"
#include "gui/imgui/component/window.h"

#include "render/opengl/texture.h"

#include "game/room.h"

namespace mixi
{
namespace gobang
{

class WindowDashboard : public imgui::Window
{
public:

    WindowDashboard(
        imgui::Context& context,
        const game::Room& room
    );
    ~WindowDashboard();

    void content() override;

private:

    const game::Room& room_;

    gl::Texture2D tex_exit_;
    gl::Texture2D tex_window_;
    gl::Texture2D tex_full_screen_;

    void load_texture_by_image_(
        gl::Texture2D& tex,
        const std::filesystem::path& filepath
    );

};

} // namespace gobang
} // namespace mixi
