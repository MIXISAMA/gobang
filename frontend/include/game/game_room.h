#pragma once
#include "pch.h"

namespace mixi
{
namespace gobang
{

class GameRoom
{
public:
    
    GameRoom(
        u_int16_t id,
        const std::string& name,
        u_int16_t max_onlookers_number
    );
    ~GameRoom() = default;

    bool is_playing() const;
    void is_playing(bool b);
    u_int16_t max_onlookers_number() const;
    const std::vector<std::string>& onlookers() const;

protected:

    const u_int16_t id_;
    const std::string name_;
    const u_int16_t max_onlookers_number_;

    bool is_playing_;
    std::vector<std::string> onlookers_;

    std::byte chess_board[16][16];
    std::vector<std::byte> record;


};

} // namespace gobang
} // namespace mixi
