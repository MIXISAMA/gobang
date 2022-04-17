#include "net/serializer.h"


namespace mixi
{
namespace gobang
{

Serializer::Serializer() :
    cursor_(0)
{

}

Serializer::Serializer(const std::vector<std::byte> &buffer) :
    raw(buffer),
    cursor_(0)
{

}

std::byte Serializer::read_byte()
{
    // Todo throw cursor exception
    return raw[cursor_++];
}

bool Serializer::read_boolean()
{
    return (bool)read_byte();
}

u_int16_t Serializer::read_uint16()
{
    u_int16_t val = 0;
    val = (u_int16_t)raw[cursor_ + 1] << 8 | (u_int16_t)raw[cursor_];
    cursor_ += 2;
    return val;
}

std::string Serializer::read_string(){
    size_t length = read_uint16();
    // throw exception
    char* buffer = (char*)raw.data();
    std::string str(buffer + cursor_, length);
    cursor_ += length;
    return str;
}

} // namespace gobang
} // namespace mixi
