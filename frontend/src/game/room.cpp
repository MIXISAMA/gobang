#include "game/room.h"
#include "game/exception.h"

namespace mixi
{
namespace gobang
{
namespace game
{

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


} // namespace game
} // namespace gobang
} // namespace mixi
