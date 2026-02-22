#pragma once

#include "types.hpp"
#include "bullet.hpp"
#include <vector>
#include <random>

class Alien {
public:
    Vec2 pos;
    int  hp           = 2;
    bool alive        = true;
    int  move_timer   = 0;
    int  move_interval = 4;  // frames between moves
    int  shoot_timer  = 0;
    int  shoot_cooldown = 55;
    float dodge_chance = 0.3f;
    int  dodge_cooldown_timer = 0;
    int  glyph_index  = 0;  // which alien glyph to use
    int  anim_timer   = 0;

    Alien() = default;
    Alien(Vec2 p, int hp_, int move_int, int shoot_cd, float dodge, int glyph);

    void update(const Vec2& player_pos, const std::vector<Bullet>& player_bullets,
                const GameArea& area, std::mt19937& rng);

    bool wants_to_shoot() const;
    Bullet shoot_at(const Vec2& player_pos, int bullet_speed);

private:
    void hunt_player(const Vec2& player_pos, const GameArea& area, std::mt19937& rng);
    bool try_dodge(const std::vector<Bullet>& player_bullets, const GameArea& area,
                   std::mt19937& rng);
};
