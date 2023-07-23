#include "pch.h"
#include "mixi/core/i18n.h"
#include "mixi/gui/application.h"
#include "component/component_root.h"


int main() {

    mixi::InitI18n("gobang", "./resource/po");
    mixi::gui::Application app(gettext("Gobang"));
    mixi::gobang::ComponentRoot root(app.context());
    app.run(&root);

    return 0;
}

