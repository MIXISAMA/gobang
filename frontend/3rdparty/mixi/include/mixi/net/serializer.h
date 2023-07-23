#pragma once

#include "mixi/pch.h"

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
    Serializer& operator >> (uint32_t&    ret);
    Serializer& operator >> (uint64_t&    ret);
    Serializer& operator >> (int32_t&     ret);
    
    Serializer& operator >> (std::string& ret);
    Serializer& operator >> (std::vector<std::byte>& ret);

    Serializer& operator << (std::byte   val);
    Serializer& operator << (bool        val);
    Serializer& operator << (uint8_t     val);
    Serializer& operator << (uint16_t    val);
    Serializer& operator << (uint32_t    val);

    Serializer& operator << (const char* val);
    Serializer& operator << (const std::string& val);
    Serializer& operator << (const std::vector<std::byte>& val);

private:

    int read_head_();
    void write_head_(int length);

    size_t cursor_;
    int head_;

};

} // namespace net
} // namespace mixi
