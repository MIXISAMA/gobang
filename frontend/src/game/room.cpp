#include "game/room.h"

namespace mixi
{
namespace gobang
{



bool Room::operator < (const Room& room) const
{
    return this->endpoint < room.endpoint;
}


} // namespace gobang
} // namespace mixi
