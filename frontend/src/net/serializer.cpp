#include "net/serializer.h"


namespace mixi
{
namespace gobang
{

Serializer::Serializer() :
    cursor_(0)
{
    raw.reserve(64);
}

Serializer::Serializer(const std::vector<std::byte> &buffer) :
    raw(buffer),
    cursor_(0)
{

}

Serializer& Serializer::operator >> (std::byte& ret) {
    if (cursor_ + 1 > raw.size()) {
        throw std::runtime_error("out of raw size");
    }
    ret = raw[cursor_++];
    return *this;
}

Serializer& Serializer::operator >> (bool& ret) {
    std::byte b;
    *this >> b;
    ret = (bool)b;
    return *this;
}

Serializer& Serializer::operator >> (u_int16_t& ret) {
    if (cursor_ + 2 > raw.size()) {
        throw std::runtime_error("out of raw size");
    }
    ret = (u_int16_t)raw[cursor_ + 1] << 8 | (u_int16_t)raw[cursor_];
    cursor_ += 2;
    return *this;
}

Serializer& Serializer::operator >> (std::string& ret) {
    u_int16_t length;
    *this >> length;
    if (cursor_ + length > raw.size()) {
        throw std::runtime_error("out of raw size");
    }
    char* buffer = (char*)raw.data();
    ret.assign(buffer + cursor_, length);
    cursor_ += length;
    return *this;
}


Serializer& Serializer::operator << (const std::byte& val)
{
    raw.push_back(val);
    return *this;
}

Serializer& Serializer::operator << (const bool& val)
{
    if (val) {
        return *this << (u_int16_t)0xFF;
    } else {
        return *this << (u_int16_t)0x00;
    }
}

Serializer& Serializer::operator << (const u_int16_t& val)
{
    return *this << (std::byte)val << (std::byte)(val >> 8);
}

Serializer& Serializer::operator << (const std::string& val)
{
    if (val.size() >= 65536) {
        throw std::runtime_error("string is too long");
    }
    u_int16_t length = val.size();
    std::byte* begin = raw.data() + raw.size();
    raw.resize(raw.size() + length + 2);
    memcpy(begin, &length, 2);
    memcpy(begin + 2, val.data(), length);
    return *this;
}

Serializer& Serializer::operator << (const char* val)
{
    return *this << std::string(val);
}

} // namespace gobang
} // namespace mixi
