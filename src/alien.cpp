#include "alien.hpp"
#include <cmath>
#include <algorithm>

Alien::Alien(Vec2 p, int hp_, int move_int, int shoot_cd, float dodge, int glyph)
    : pos(p), hp(hp_), move_interval(move_int), shoot_cooldown(shoot_cd),
      dodge_chance(dodge), glyph_index(glyph) {}

void Alien::update(const Vec2& player_pos, const std::vector<Bullet>& player_bullets,
                   const GameArea& area, std::mt19937& rng) {
    if (!alive) return;

    anim_timer++;
    if (dodge_cooldown_timer > 0) dodge_cooldown_timer--;
    if (shoot_timer > 0) shoot_timer--;

    move_timer++;
    if (move_timer < move_interval) return;
    move_timer = 0;

    // Priority 1: Try to dodge incoming bullets
    if (!try_dodge(player_bullets, area, rng)) {
        // Priority 2: Hunt the player
        hunt_player(player_pos, area, rng);
    }
}

void Alien::hunt_player(const Vec2& player_pos, const GameArea& area, std::mt19937& rng) {
    int dx = player_pos.x - pos.x;
    int dy = player_pos.y - pos.y;

    // Add some randomness to movement to avoid perfectly linear paths
    std::uniform_int_distribution<int> jitter(-1, 1);

    int move_x = 0, move_y = 0;

    // Primarily move along the axis with greater distance
    if (std::abs(dx) > std::abs(dy)) {
        move_x = (dx > 0) ? 1 : -1;
        // Sometimes also move on minor axis for diagonal movement
        if (std::abs(dy) > 2) {
            move_y = (dy > 0) ? 1 : -1;
        }
    } else if (std::abs(dy) > 0) {
        move_y = (dy > 0) ? 1 : -1;
        if (std::abs(dx) > 2) {
            move_x = (dx > 0) ? 1 : -1;
        }
    }

    // Add jitter occasionally for unpredictable movement
    if (jitter(rng) == 0) {
        move_x += jitter(rng);
        move_y += jitter(rng);
        move_x = std::clamp(move_x, -1, 1);
        move_y = std::clamp(move_y, -1, 1);
    }

    int new_x = pos.x + move_x;
    int new_y = pos.y + move_y;

    // Bounds check
    if (new_x > area.left && new_x < area.right)  pos.x = new_x;
    if (new_y > area.top  && new_y < area.bottom)  pos.y = new_y;
}

bool Alien::try_dodge(const std::vector<Bullet>& player_bullets, const GameArea& area,
                      std::mt19937& rng) {
    if (dodge_cooldown_timer > 0) return false;

    const int THREAT_RADIUS = 6;

    for (const auto& b : player_bullets) {
        if (!b.alive || b.owner != Owner::Player) continue;

        // Check if bullet is heading toward us and close enough
        int dist_x = std::abs(b.pos.x - pos.x);
        int dist_y = std::abs(b.pos.y - pos.y);

        bool on_collision_course = false;

        // Bullet moving vertically toward alien
        if (b.dx == 0 && std::abs(b.pos.x - pos.x) <= 1) {
            if ((b.dy < 0 && b.pos.y > pos.y && dist_y <= THREAT_RADIUS) ||
                (b.dy > 0 && b.pos.y < pos.y && dist_y <= THREAT_RADIUS)) {
                on_collision_course = true;
            }
        }
        // Bullet moving horizontally toward alien
        if (b.dy == 0 && std::abs(b.pos.y - pos.y) <= 1) {
            if ((b.dx < 0 && b.pos.x > pos.x && dist_x <= THREAT_RADIUS) ||
                (b.dx > 0 && b.pos.x < pos.x && dist_x <= THREAT_RADIUS)) {
                on_collision_course = true;
            }
        }

        if (!on_collision_course) continue;

        // Roll dodge chance
        std::uniform_real_distribution<float> roll(0.0f, 1.0f);
        if (roll(rng) > dodge_chance) continue;

        // Dodge perpendicular to bullet direction
        int dodge_x = 0, dodge_y = 0;
        if (b.dx == 0) {
            // Bullet moves vertically → dodge horizontally
            dodge_x = (rng() % 2 == 0) ? 2 : -2;
        } else {
            // Bullet moves horizontally → dodge vertically
            dodge_y = (rng() % 2 == 0) ? 2 : -2;
        }

        int new_x = pos.x + dodge_x;
        int new_y = pos.y + dodge_y;

        // Bounds check for dodge
        if (new_x > area.left && new_x < area.right)  pos.x = new_x;
        if (new_y > area.top  && new_y < area.bottom)  pos.y = new_y;

        dodge_cooldown_timer = 8; // Prevent constant dodging
        return true;
    }

    return false;
}

bool Alien::wants_to_shoot() const {
    return alive && shoot_timer <= 0;
}

Bullet Alien::shoot_at(const Vec2& player_pos, int bullet_speed) {
    shoot_timer = shoot_cooldown;

    int dx_dir = player_pos.x - pos.x;
    int dy_dir = player_pos.y - pos.y;

    int bx = 0, by = 0;

    // Shoot in the dominant direction toward player
    if (std::abs(dx_dir) > std::abs(dy_dir)) {
        bx = (dx_dir > 0) ? 1 : -1;
    } else {
        by = (dy_dir > 0) ? 1 : -1;
    }

    // If very close, shoot directly
    if (std::abs(dx_dir) <= 3 && std::abs(dy_dir) <= 3) {
        if (std::abs(dx_dir) > 0) bx = (dx_dir > 0) ? 1 : -1;
        else by = (dy_dir > 0) ? 1 : -1;
    }

    Vec2 bullet_pos = {pos.x + bx, pos.y + by};
    return Bullet(bullet_pos, bx, by, Owner::Alien, bullet_speed);
}
