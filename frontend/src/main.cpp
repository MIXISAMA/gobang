#include "core/i18n.h"

#include "gui/imgui/base/application.h"
#include "component/component_root.h"

int main() {
    mixi::InitI18n("gobang", "./resource/po");
    mixi::imgui::Application app(gettext("Gobang"));
    mixi::gobang::ComponentRoot root;
    app.loop(&root);
    return 0;
}