#include "bullet.hpp"

void Bullet::update(const GameArea& area) {
    move_timer++;
    if (move_timer < move_interval) return;
    move_timer = 0;

    pos.x += dx;
    pos.y += dy;

    if (pos.x <= area.left || pos.x >= area.right ||
        pos.y <= area.top  || pos.y >= area.bottom) {
        alive = false;
    }
}
