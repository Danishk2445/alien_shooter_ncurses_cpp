#pragma once

#include "types.hpp"

class Bullet {
public:
    Vec2  pos;
    int   dx = 0;
    int   dy = 0;
    Owner owner;
    bool  alive = true;
    int   move_timer = 0;
    int   move_interval = 1; // frames between moves

    Bullet() = default;
    Bullet(Vec2 p, int dx_, int dy_, Owner o, int interval = 1)
        : pos(p), dx(dx_), dy(dy_), owner(o), move_interval(interval) {}

    void update(const GameArea& area);
};
