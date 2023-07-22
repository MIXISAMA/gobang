#pragma once
#include "pch.h"
#include "game/chess.h"

namespace mixi
{
namespace gobang
{
namespace game
{

const std::byte BLACK{0x00};
const std::byte WHITE{0x01};
const std::byte SPACE{0xFF};

struct User
{
    std::string username;
    uint32_t number_of_wins;
    uint32_t number_of_ties;
    uint32_t number_of_losses;
    uint32_t number_of_matches;
    uint64_t game_duration;
};

struct Room
{

    std::string name;
    int max_users;

    std::string black_player;
    std::string white_player;

    std::vector<std::string> onlookers;

    bool is_playering;

    std::byte ready_player;
    std::byte regret_player;
    std::byte tie_player;

    std::byte board[15][15];
    std::vector<std::byte> records;

    void user_join(const std::string& username, std::byte role);
    void user_leave(const std::string& username);

    void stone(std::byte coor);

    void update_board_by_records();

};

} // namespace game
} // namespace gobang
} // namespace mixi
