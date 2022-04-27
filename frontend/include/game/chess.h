#pragma once
#include "pch.h"

namespace mixi
{
namespace gobang
{
namespace game
{

class Chess
{

public:

    enum class Color
    {
        BLACK = 0x00,
        WHITE = 0x01,
    };

    static const std::byte SPACE;

    std::byte board[15][15];
    std::vector<std::byte> record;

    Chess();
    ~Chess() = default;

    Color whos_turn() const;

    void stone(std::byte coor, Color color);

    void clean();

};

} // namespace game
} // namespace gobang
} // namespace mixi
