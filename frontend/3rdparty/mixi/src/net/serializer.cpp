#include "net/serializer.h"


namespace mixi
{
namespace net
{

Serializer::Serializer() :
    cursor_(0),
    head_(8)
{
    raw.reserve(64);
}

Serializer::Serializer(const std::vector<std::byte> &buffer) :
    raw(buffer),
    cursor_(0)
{

}

void Serializer::head8() {
    head_ = 8;
}

void Serializer::head16() {
    head_ = 16;
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

Serializer& Serializer::operator >> (uint8_t& ret) {
    *this >> (std::byte&)ret;
    return *this;
}

Serializer& Serializer::operator >> (uint16_t& ret) {
    if (cursor_ + 2 > raw.size()) {
        throw std::runtime_error("out of raw size");
    }
    ret = (uint16_t)raw[cursor_ + 1] << 8 | (uint16_t)raw[cursor_];
    cursor_ += 2;
    return *this;
}

Serializer& Serializer::operator >> (int32_t& ret) {
    if (cursor_ + 4 > raw.size()) {
        throw std::runtime_error("out of raw size");
    }
    ret = (int32_t)raw[cursor_ + 3] << 24
        | (int32_t)raw[cursor_ + 2] << 16
        | (int32_t)raw[cursor_ + 1] << 8
        | (int32_t)raw[cursor_];
    cursor_ += 4;
    return *this;
}

Serializer& Serializer::operator >> (std::string& ret) {
    int length = read_head_();
    char* buffer = (char*)raw.data();
    ret.assign(buffer + cursor_, length);
    cursor_ += length;
    return *this;
}

Serializer& Serializer::operator >> (std::vector<std::byte>& ret) {
    int length = read_head_();
    ret.assign(raw.begin() + cursor_, raw.begin() + cursor_ + length);
    cursor_ += length;
    return *this;
}


Serializer& Serializer::operator << (std::byte val)
{
    raw.push_back(val);
    return *this;
}

Serializer& Serializer::operator << (bool val)
{
    if (val) {
        return *this << (u_int16_t)0xFF;
    } else {
        return *this << (u_int16_t)0x00;
    }
}

Serializer& Serializer::operator << (uint8_t val)
{
    raw.push_back((std::byte)val);
    return *this;
}

Serializer& Serializer::operator << (uint16_t val)
{
    return *this << (std::byte)val << (std::byte)(val >> 8);
}

Serializer& Serializer::operator << (const std::string& val)
{
    int length = val.size();
    write_head_(length);
    raw.resize(raw.size() + length);
    memcpy(raw.data() + raw.size(), val.data(), length);
    return *this;
}

Serializer& Serializer::operator << (const std::vector<std::byte>& val)
{
    int length = val.size();
    write_head_(length);
    raw.resize(raw.size() + length);
    memcpy(raw.data() + raw.size(), val.data(), length);
    return *this;
}

Serializer& Serializer::operator << (const char* val)
{
    return *this << std::string(val);
}

int Serializer::read_head_()
{
    int length;
    if (head_ == 8) {
        uint8_t n;
        *this >> n;
        length = n;
    }
    else if (head_ == 16) {
        uint16_t n;
        *this >> n;
        length = n;
    }
    else {
        throw std::runtime_error("wrong head");
    }
    if (cursor_ + length > raw.size()) {
        throw std::runtime_error("out of raw size");
    }
    return length;
}

void Serializer::write_head_(int length)
{
    if (head_ == 8) {
        if (length > 0xFF) {
            throw std::runtime_error("string is too long");
        }
        *this << (uint8_t)length;
        return;
    }
    if (head_ == 16) {
        if (length > 0xFFFF) {
            throw std::runtime_error("string is too long");
        }
        *this << (uint16_t)length;
        return;
    }
    assert(false && "wrong head");
}

} // namespace net
} // namespace mixi
