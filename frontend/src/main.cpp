#include "pch.h"
#include "core/i18n.h"
#include "gui/imgui/base/application.h"
#include "component/component_root.h"
#include "net/udp_server.h"


int main() {

    boost::asio::io_context io_context;
    mixi::gobang::UdpServer::Init(io_context);
    std::thread server([&io_context](){
        io_context.run();
        std::cout << "wondeful!" << std::endl;
    });

    mixi::InitI18n("gobang", "./resource/po");
    mixi::imgui::Application app(gettext("Gobang"));
    mixi::gobang::ComponentRoot root;
    app.loop(&root);

    return 0;
}
