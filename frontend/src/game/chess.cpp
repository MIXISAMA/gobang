#include "game/chess.h"
#include "game/exception.h"

namespace mixi
{
namespace gobang
{
namespace game
{

const std::byte Chess::SPACE = std::byte(0xFF);

Chess::Chess()
{
    memset(board, (int)SPACE, sizeof(board));
}

Chess::Color Chess::whos_turn() const
{
    return (Color)(record.size() % 2);
}

void Chess::stone(std::byte coor, Color color)
{

    if (color != whos_turn()) {
        throw Exception();
    }

    int r = (int)coor & 0x0F;
    int c = ((int)coor >> 4) & 0x0F;

    if (r >= 15 || c >= 15) {
        throw Exception();
    }
    if (board[r][c] != SPACE) {
        throw Exception();
    }

    record.push_back(coor);
    board[r][c] = (std::byte)color;

}

} // namespace game
} // namespace gobang
} // namespace mixi
