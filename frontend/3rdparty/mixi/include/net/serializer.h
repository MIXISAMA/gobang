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

    void head8();
    void head16();

    Serializer& operator >> (std::byte&   ret);
    Serializer& operator >> (bool&        ret);
    Serializer& operator >> (uint8_t&     ret);
    Serializer& operator >> (uint16_t&    ret);
    Serializer& operator >> (int32_t&     ret);
    
    Serializer& operator >> (std::string& ret);
    Serializer& operator >> (std::vector<std::byte>& ret);

    Serializer& operator << (const std::byte&   val);
    Serializer& operator << (const bool&        val);
    Serializer& operator << (const u_int16_t&   val);
    Serializer& operator << (const std::string& val);
    Serializer& operator << (const char*        val);

private:

    size_t cursor_;
    int head_;

};

} // namespace net
} // namespace mixi
