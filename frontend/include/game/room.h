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

struct Player : public User
{
    using Ptr = std::shared_ptr<Player>;
    Chess::Color color;
    bool is_ready;
    bool is_repentant;
};


class Room
{

public:

    Room();
    ~Room() = default;

    std::string name() const;
    void name(const std::string& s);

    int max_onlookers() const;
    void max_onlookers(int m);

    bool is_playing() const;
    void is_playing(bool b);

    Player::Ptr self() const;
    void self(Player::Ptr p);
    
    Player::Ptr opponent() const;
    void opponent(Player::Ptr p);

    void copy_onlookers(
        std::vector<User::Ptr>& onlookers,
        long long& onlookers_version
    );

    void onlooker_join(const std::string& name);
    void onlooker_leave(const std::string& name);

    Chess& chess();

protected:

    std::string name_;
    int max_onlookers_;

    bool is_playing_;
    Player::Ptr self_;
    Player::Ptr opponent_;

    std::vector<User::Ptr> onlookers_;
    std::mutex onlookers_mutex_;
    long long onlookers_version_;

    Chess chess_;

};

} // namespace game
} // namespace gobang
} // namespace mixi
