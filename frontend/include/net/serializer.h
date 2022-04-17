#pragma once

#include "pch.h"

namespace mixi
{
namespace gobang
{

class Serializer
{

public:

    std::vector<std::byte> raw;

    Serializer();
    Serializer(const std::vector<std::byte> &buffer);
    ~Serializer() = default;

    std::byte read_byte();
    bool read_boolean();
    u_int16_t read_uint16();
    std::string read_string();

private:

    size_t cursor_;

};

} // namespace gobang
} // namespace mixi
