#include "game/room.h"
#include "game/exception.h"

namespace mixi
{
namespace gobang
{
namespace game
{

Room::Room() : 
    max_onlookers_(16),
    is_playing_(false)
{

}

bool Room::is_playing() const
{
    return is_playing_;
}

void Room::is_playing(bool b)
{
    is_playing_ = b;
}

std::string Room::name() const
{
    return name_;
}

void Room::name(const std::string& s)
{
    name_ = s;
}

int Room::max_onlookers() const
{
    return max_onlookers_;
}

void Room::max_onlookers(int m)
{
    max_onlookers_ = m;
}

Player::Ptr Room::self() const
{
    return self_;
}

void Room::self(Player::Ptr p)
{
    self_ = p;
}

Player::Ptr Room::opponent() const
{
    return opponent_;
}

void Room::opponent(Player::Ptr p)
{
    opponent_ = p;
}

void Room::copy_onlookers(
    std::vector<User::Ptr>& onlookers,
    long long& onlookers_version
) {
    if (!onlookers_mutex_.try_lock()) {
        return;
    }
    if (onlookers_version != onlookers_version_) {
        onlookers_version = onlookers_version_;
        onlookers.assign(onlookers_.begin(), onlookers_.end());
    }
    onlookers_mutex_.unlock();
}

void Room::onlooker_join(const std::string& name)
{
    onlookers_mutex_.lock();
    onlookers_.push_back(User::Ptr(new User{.name = name}));
    onlookers_version_++;
    onlookers_mutex_.unlock();
}

void Room::onlooker_leave(const std::string& name)
{
    onlookers_mutex_.lock();
    for (auto iter = onlookers_.begin(); iter != onlookers_.end(); iter++) {
        if ((*iter)->name == name) {
            onlookers_.erase(iter);
            onlookers_version_++;
            break;
        }
    }
    onlookers_mutex_.unlock();
}

Chess& Room::chess()
{
    return chess_;
}

} // namespace game
} // namespace gobang
} // namespace mixi
