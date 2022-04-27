#include "pch.h"
#include "core/i18n.h"
#include "gui/imgui/application.h"
#include "component/component_root.h"


int main() {

    mixi::InitI18n("gobang", "./resource/po");
    mixi::imgui::Application app(gettext("Gobang"));
    mixi::gobang::ComponentRoot root(app.context());
    app.loop(&root, 1000 / 30);

    return 0;
}
