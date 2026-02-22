#include "player.hpp"
#include <ncurses.h>

void Player::update(int ch, const GameArea& area) {
    if (shoot_cooldown > 0) shoot_cooldown--;
    if (invincible_timer > 0) invincible_timer--;

    switch (ch) {
        case 'w': case 'W': case KEY_UP:
            facing = Direction::Up;
            if (pos.y > area.top + 1) pos.y--;
            break;
        case 's': case 'S': case KEY_DOWN:
            facing = Direction::Down;
            if (pos.y < area.bottom - 1) pos.y++;
            break;
        case 'a': case 'A': case KEY_LEFT:
            facing = Direction::Left;
            if (pos.x > area.left + 1) pos.x--;
            break;
        case 'd': case 'D': case KEY_RIGHT:
            facing = Direction::Right;
            if (pos.x < area.right - 1) pos.x++;
            break;
    }
}

bool Player::can_shoot() const {
    return ammo > 0 && shoot_cooldown <= 0 && alive;
}

Bullet Player::shoot() {
    ammo--;
    shoot_cooldown = 6;

    int dx = 0, dy = 0;
    switch (facing) {
        case Direction::Up:    dy = -1; break;
        case Direction::Down:  dy =  1; break;
        case Direction::Left:  dx = -1; break;
        case Direction::Right: dx =  1; break;
        default: dy = -1; break;
    }

    Vec2 bullet_pos = pos;
    bullet_pos.x += dx;
    bullet_pos.y += dy;

    return Bullet(bullet_pos, dx, dy, Owner::Player, 1);
}

void Player::take_damage(int dmg) {
    if (invincible_timer > 0) return;
    health -= dmg;
    invincible_timer = 20; // brief invincibility
    if (health <= 0) {
        health = 0;
        alive = false;
    }
}

void Player::collect_ammo(int amount) {
    ammo += amount;
}

void Player::collect_health(int amount) {
    health += amount;
    if (health > max_health) health = max_health;
}
