#include "core/io_context.h"

namespace mixi
{

boost::asio::io_context* Io_Context = nullptr;

void CreateIoContext()
{
    Io_Context = new(boost::asio::io_context);
}

void DestroyIoContext()
{
    delete(Io_Context);
}

} // namespace mixi
