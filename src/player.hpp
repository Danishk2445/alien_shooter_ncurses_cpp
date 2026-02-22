#pragma once

#include "types.hpp"
#include "bullet.hpp"
#include <vector>

class Player {
public:
    Vec2 pos;
    int  health    = 100;
    int  max_health = 100;
    int  ammo      = 30;
    int  score     = 0;
    Direction facing = Direction::Up;
    bool alive      = true;
    int  shoot_cooldown = 0;
    int  invincible_timer = 0; // brief invincibility after hit

    Player() = default;
    explicit Player(Vec2 start) : pos(start) {}

    void update(int ch, const GameArea& area);
    bool can_shoot() const;
    Bullet shoot();
    void take_damage(int dmg);
    void collect_ammo(int amount);
    void collect_health(int amount);
};
