#pragma once

#include "types.hpp"

class Pickup {
public:
    Vec2       pos;
    PickupType type;
    bool       alive    = true;
    int        lifetime = 300; // frames before despawn
    int        blink_timer = 0;

    Pickup() = default;
    Pickup(Vec2 p, PickupType t) : pos(p), type(t) {}

    void update();
    int  get_value() const;
    bool is_visible() const; // for blink effect
};
