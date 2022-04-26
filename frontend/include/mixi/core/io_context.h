#pragma once

#include "pch.h"

namespace mixi
{

extern boost::asio::io_context* Io_Context;

void CreateIoContext();
void DestroyIoContext();

} // namespace mixi
