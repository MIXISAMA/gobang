#pragma once
#include "pch.h"
#include "mixi/gui/component.h"
#include "mixi/render/opengl/texture.h"

#include "server/server_game_room.h"
#include "component/modal_confirm_leave.h"

namespace mixi
{
namespace gobang
{

class WindowDashboard : public gui::Window
{
public:

    WindowDashboard(
        gui::Context& context,
        const ReadFirstBuffer<game::Room>& room
    );

    void role(std::byte r);

    void on_leave    (const std::function<void()>& f);
    void on_regret   (const std::function<void()>& f);
    void on_tie      (const std::function<void()>& f);
    void on_give_up  (const std::function<void()>& f);
    void on_user_info(const std::function<void(const std::string&)>& f);

protected:

    void content() override;

private:

    const ReadFirstBuffer<game::Room>& room_;

    gl::Texture2D tex_exit_;
    gl::Texture2D tex_window_;
    gl::Texture2D tex_full_screen_;

    ModalConfirmLeave modal_confirm_leave_;

    std::function<void()> on_regret_;
    std::function<void()> on_tie_;
    std::function<void()> on_give_up_;
    std::function<void(const std::string&)> on_user_info_;

    std::byte role_;

    void load_texture_by_image_(
        gl::Texture2D& tex,
        const std::filesystem::path& filepath
    );

};

} // namespace gobang
} // namespace mixi
