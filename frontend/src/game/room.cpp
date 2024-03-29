#include "game/room.h"

namespace mixi
{
namespace gobang
{
namespace game
{

Room::Room() : board{SPACE} {}

std::byte Room::role(const std::string& username) const
{
    if (username == black_player) {
        return BLACK;
    }
    if (username == white_player) {
        return WHITE;
    }
    return SPACE;
}

void Room::user_join(const std::string& username, std::byte role)
{
    if (role == BLACK) {
        black_player = username;
    }
    else if (role == WHITE) {
        white_player = username;
    }
    else {
        onlookers.emplace_back(username);
    }
}

void Room::user_leave(const std::string& username)
{
    if (black_player == username) {
        black_player.clear();
    }
    else if (white_player == username) {
        white_player.clear();
    }
    else {
        for (auto iter = onlookers.begin(); iter != onlookers.end(); iter++) {
            if (*iter == username) {
                onlookers.erase(iter);
                break;
            }
        }
    }
}

void Room::stone(std::byte coor)
{
    int r = 0x0F & (int)(coor >> 4);
    int c = 0x0F & (int)coor;
    board[r][c] = (std::byte)(records.size() % 2);
    records.push_back(coor);
}

void Room::clear_board()
{
    std::fill(board[0], board[0] + 15 * 15, SPACE);
}

void Room::set_board_by_records()
{
    clear_board();
    for (int i = 0; i < records.size(); i++) {
        std::byte coor = records[i];
        int r = 0xF & (int)(coor >> 4);
        int c = 0xF & (int)coor;
        board[r][c] = (std::byte)(records.size() % 2);
    }
}


} // namespace game
} // namespace gobang
} // namespace mixi
