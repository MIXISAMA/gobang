#pragma once
#include "pch.h"



namespace mixi
{
namespace gobang
{

class FileReverseReader
{
public:

    FileReverseReader(const std::filesystem::path& filepath);
    ~FileReverseReader();

    bool getline(std::string& line);

protected:

    std::ifstream file_;
    std::streampos pos_;

};

} // namespace gobang
} // namespace mixi
