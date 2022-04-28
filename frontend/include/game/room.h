#pragma once
#include "pch.h"
#include "game/chess.h"

namespace mixi
{
namespace gobang
{
namespace game
{

struct User
{
    using Ptr = std::shared_ptr<User>;

    std::string name;
};

class Room
{

public:
    
    Room(
        const std::string& name,
        u_int16_t max_users
    );
    ~Room() = default;

    bool is_playing() const;
    void is_playing(bool b);

    const std::string& name() const;
    u_int16_t max_users() const;

    std::vector<User::Ptr>& users();
    const std::vector<User::Ptr>& users() const;

    User::Ptr user(const std::string& name) const;

    User::Ptr player(Chess::Color color) const;
    void player(Chess::Color color, const User::Ptr& player);

    User::Ptr repentant() const;
    void repentant(const User::Ptr& user);

    User::Ptr ready_user() const;
    void ready_user(const User::Ptr& user);

    Chess& chess();

    void onlooker_join(const std::string& name);    
    void player_join(const std::string& name, Chess::Color color);
    void user_leave(const std::string& name);

    void stone(const std::string& name, std::byte coor);

protected:

    const std::string name_;
    const u_int16_t max_users_;

    bool is_playing_;
    std::vector<User::Ptr> users_;
    User::Ptr players_[2];
    User::Ptr repentant_;
    User::Ptr ready_user_;

    Chess chess_;

    std::vector<User::Ptr>::const_iterator
    user_iter_(const std::string& name) const;

    void clean_();

    void onlooker_join_(const User::Ptr& user);
    void player_join_(const User::Ptr& player, Chess::Color color);

};

} // namespace game
} // namespace gobang
} // namespace mixi
