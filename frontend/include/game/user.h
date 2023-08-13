#pragma once
#include "pch.h"

namespace mixi
{
namespace gobang
{
namespace game
{

struct User
{
    std::string username;
    uint32_t number_of_wins;
    uint32_t number_of_ties;
    uint32_t number_of_losses;
    uint32_t number_of_matches;
    uint32_t game_duration;
};

} // namespace game
} // namespace gobang
} // namespace mixi
