#include "game/room.h"
#include "game/exception.h"

namespace mixi
{
namespace gobang
{
namespace game
{

Room::Room(
    const std::string& name,
    u_int16_t max_users
) : 
    name_(name),
    max_users_(max_users),
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

const std::string& Room::name() const
{
    return name_;
}

u_int16_t Room::max_users() const
{
    return max_users_;
}

std::vector<User::Ptr>& Room::users()
{
    return users_;
}

const std::vector<User::Ptr>& Room::users() const
{
    return users_;
}

User::Ptr Room::user(const std::string& name) const
{
    auto iter = user_iter_(name);
    if (iter == users_.end()) {
        return nullptr;
    }
    return *iter;
}

User::Ptr Room::player(Chess::Color color) const
{
    return players_[(int)color];
}

void Room::player(Chess::Color color, const User::Ptr& player)
{
    players_[(int)color] = player;
}

User::Ptr Room::repentant() const
{
    return repentant_;
}

void Room::repentant(const User::Ptr& user)
{
    repentant_ = user;
}

User::Ptr Room::ready_user() const
{
    return ready_user_;
}

void Room::ready_user(const User::Ptr& user)
{
    ready_user_ = user;
}

Chess& Room::chess()
{
    return chess_;
}

void Room::onlooker_join_(const User::Ptr& user)
{
    users_.push_back(user);
}

void Room::onlooker_join(const std::string& name)
{
    // no exception, because of thin client

    User::Ptr user = User::Ptr(new User{.name = name});
    onlooker_join_(user);
}

void Room::player_join_(const User::Ptr& player, Chess::Color color)
{
    players_[(int)color] = player;
    onlooker_join_(player);
}

void Room::player_join(const std::string& name, Chess::Color color)
{
    User::Ptr player = User::Ptr(new User{.name = name});
    player_join_(player, color);
}

void Room::user_leave(const std::string& name)
{
 
    auto iter = user_iter_(name);
    User::Ptr user = *iter;
    users_.erase(iter);
    for (int i = 0; i < 2; i++) {
        if (players_[i] == user) {
            players_[i].reset();
            clean_();
        }
    }
    if (repentant_ == user) {
        repentant_.reset();
    }
    if (ready_user_ == user) {
        ready_user_.reset();
    }

}

void Room::stone(const std::string& name, std::byte coor)
{
    for (int i = 0; i < 2; i++) {
        if (players_[i]->name == name) {
            Chess::Color color = (Chess::Color)i;
            chess_.stone(coor, color);
            break;
        }
    }
}

std::vector<User::Ptr>::const_iterator
Room::user_iter_(const std::string& name) const
{
    return std::find_if(
        users_.begin(),
        users_.end(),
        [name](const User::Ptr& user){
            return user->name == name;
        }
    );
}

void Room::clean_()
{
    is_playing_ = false;
    repentant_.reset();
    ready_user_.reset();
    chess_.clean();
    std::swap(players_[0], players_[1]);
}

} // namespace game
} // namespace gobang
} // namespace mixi
