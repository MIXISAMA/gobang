#pragma once

#include "pch.h"

namespace mixi
{
namespace net
{

class Serializer
{

public:

    std::vector<std::byte> raw;

    Serializer();
    Serializer(const std::vector<std::byte>& buffer);
    ~Serializer() = default;

    Serializer& operator >> (std::byte&   ret);
    Serializer& operator >> (bool&        ret);
    Serializer& operator >> (u_int16_t&   ret);
    Serializer& operator >> (std::string& ret);

    Serializer& operator << (const std::byte&   val);
    Serializer& operator << (const bool&        val);
    Serializer& operator << (const u_int16_t&   val);
    Serializer& operator << (const std::string& val);
    Serializer& operator << (const char*        val);

private:

    size_t cursor_;

};

} // namespace net
} // namespace mixi
