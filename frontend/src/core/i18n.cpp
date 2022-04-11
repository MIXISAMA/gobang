#include "core/i18n.h"

namespace mixi
{

void InitI18n(const char *domain_name, const char *dir_name)
{
    setlocale(LC_ALL, "");

    bindtextdomain(domain_name, dir_name);
    textdomain(domain_name);
}

} // namespace mixi
