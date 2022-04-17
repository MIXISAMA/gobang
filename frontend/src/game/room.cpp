#include "game/room.h"

namespace mixi
{
namespace gobang
{



bool Room::operator < (const Room& room) const
{
    if (this->endpoint == room.endpoint) {
        return this->id < room.id;
    }
    return this->endpoint < room.endpoint;
}


} // namespace gobang
} // namespace mixi
