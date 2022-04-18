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

Serializer& Serializer::operator >> (std::byte& ret) {
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
    ret = (u_int16_t)raw[cursor_ + 1] << 8 | (u_int16_t)raw[cursor_];
    cursor_ += 2;
    return *this;
}

Serializer& Serializer::operator >> (std::string& ret) {
    u_int16_t length;
    // throw exception
    *this >> length;
    char* buffer = (char*)raw.data();
    ret.assign(buffer + cursor_, length);
    cursor_ += length;
    return *this;
}


// std::byte Serializer::read_byte_()
// {
//     // Todo throw cursor exception
//     return raw[cursor_++];
// }

// bool Serializer::read_boolean_()
// {
//     return (bool)read_byte_();
// }

// u_int16_t Serializer::read_uint16()
// {
//     u_int16_t val = 0;
//     val = (u_int16_t)raw[cursor_ + 1] << 8 | (u_int16_t)raw[cursor_];
//     cursor_ += 2;
//     return val;
// }

// std::string Serializer::read_string(){
//     size_t length = read_uint16();
//     // throw exception
//     char* buffer = (char*)raw.data();
//     std::string str(buffer + cursor_, length);
//     cursor_ += length;
//     return str;
// }


} // namespace gobang
} // namespace mixi
