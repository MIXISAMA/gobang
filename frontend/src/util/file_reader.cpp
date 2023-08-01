#include "util/file_reader.h"

namespace mixi
{
namespace gobang
{

FileReverseReader::FileReverseReader(const std::filesystem::path& filepath) :
    file_(filepath, std::ios::ate),
    pos_(file_.tellg())
{

}

FileReverseReader::~FileReverseReader()
{
    file_.close();
}

bool FileReverseReader::getline(std::string& line)
{
    if (pos_ < 0) {
        return false;
    }
    std::streampos pioneer_pos = pos_ - std::streamoff(1);
    for (; pioneer_pos >= 0; pioneer_pos -= 1) {
        char ch;
        file_.seekg(pioneer_pos);
        file_.get(ch);
        if (ch == '\n') {
            break;
        }
    }
    std::streampos begin_pos = pioneer_pos + std::streamoff(1);
    int length = pos_ - begin_pos;
    line.resize(length + 1);
    file_.seekg(begin_pos);
    file_.read(line.data(), length);
    line.resize(length);

    pos_ = pioneer_pos;
    return true;
}

} // namespace gobang
} // namespace mixi
