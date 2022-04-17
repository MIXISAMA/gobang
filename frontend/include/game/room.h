#pragma once

#include "pch.h"

namespace mixi
{
namespace gobang
{

class Room
{

public:

    int id;
    std::string name;
    boost::asio::ip::tcp::endpoint endpoint;

    bool operator < (const Room& room) const;
};

} // namespace gobang
} // namespace mixi
