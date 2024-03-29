#pragma once
#include "pch.h"

namespace mixi
{
namespace gobang
{
namespace game
{

const std::byte BLACK{0x00};
const std::byte WHITE{0x01};
const std::byte SPACE{0xFF};

struct Room
{

    std::string name;
    uint8_t max_users;

    std::string black_player;
    std::string white_player;

    std::vector<std::string> onlookers;

    bool is_playering;

    std::byte ready_player;
    std::byte regret_player;
    std::byte tie_player;

    std::byte board[15][15];
    std::vector<std::byte> records;

    Room();

    std::byte role(const std::string& username) const;

    void user_join(const std::string& username, std::byte role);
    void user_leave(const std::string& username);

    void stone(std::byte coor);

    void clear_board();
    void set_board_by_records();

};

} // namespace game
} // namespace gobang
} // namespace mixi
