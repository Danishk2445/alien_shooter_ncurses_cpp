#pragma once

#include <cstdint>
#include <string>

// ── Color pair IDs for ncurses ──────────────────────────────────────────────
enum ColorPair : int {
    CP_CYAN = 1,
    CP_MAGENTA,
    CP_GREEN,
    CP_YELLOW,
    CP_RED,
    CP_WHITE,
    CP_BLUE,
    CP_NEON_PINK,      // magenta on black, bold
    CP_NEON_GREEN,     // green on black, bold
    CP_NEON_CYAN,      // cyan on black, bold
    CP_HUD_BG,         // white on blue
    CP_HEALTH_HIGH,    // green
    CP_HEALTH_MED,     // yellow
    CP_HEALTH_LOW,     // red
    CP_TITLE,          // bold magenta
    CP_MENU_SELECTED,  // bold cyan
    CP_MENU_NORMAL,    // white
    CP_BORDER,         // cyan
    CP_EXPLOSION,      // yellow+bold
    CP_PICKUP_AMMO,    // green blink
    CP_PICKUP_HEALTH,  // red
    CP_SCORE,          // yellow
    CP_WAVE,           // magenta
};

// ── Direction ───────────────────────────────────────────────────────────────
enum class Direction { Up, Down, Left, Right, None };

// ── Owner tag ───────────────────────────────────────────────────────────────
enum class Owner { Player, Alien };

// ── Pickup type ─────────────────────────────────────────────────────────────
enum class PickupType { Ammo, Health };

// ── Difficulty ──────────────────────────────────────────────────────────────
enum class Difficulty { Easy, Medium, Hard };

struct DifficultyConfig {
    float enemy_speed_mult;     // multiplier on alien move interval
    int   spawn_interval;       // frames between spawns
    float dodge_chance;         // 0..1 chance alien dodges a bullet
    int   alien_hp;             // hit points per alien
    float ammo_drop_rate;       // 0..1 chance of ammo drop on kill
    int   alien_shoot_cooldown; // frames between alien shots
    int   score_multiplier;     // points per kill multiplier
    int   max_aliens;           // max simultaneous aliens
    float health_drop_rate;     // 0..1 chance of health drop on kill
    int   alien_bullet_speed;   // frames between alien bullet moves
    std::string label;
};

inline DifficultyConfig get_difficulty_config(Difficulty d) {
    switch (d) {
        //          speed_mult  spawn_int  dodge   hp  ammo_drop  shoot_cd  score_mult  max_aliens  hp_drop  bullet_spd  label
        case Difficulty::Easy:
            return {2.5f,       120,       0.05f,  1,  0.55f,     110,      1,          5,          0.20f,   4,          "EASY"};
        case Difficulty::Medium:
            return {1.8f,       80,        0.15f,  2,  0.35f,     75,       2,          8,          0.12f,   3,          "MEDIUM"};
        case Difficulty::Hard:
            return {1.2f,       55,        0.30f,  3,  0.25f,     55,       3,          12,         0.08f,   2,          "HARD"};
    }
    return {1.8f, 80, 0.15f, 2, 0.35f, 75, 2, 8, 0.12f, 3, "MEDIUM"};
}

// ── Vec2 ────────────────────────────────────────────────────────────────────
struct Vec2 {
    int x = 0;
    int y = 0;
    bool operator==(const Vec2& o) const { return x == o.x && y == o.y; }
    bool operator!=(const Vec2& o) const { return !(*this == o); }
};

// ── Game area (set at runtime from terminal size) ───────────────────────────
struct GameArea {
    int left   = 1;
    int top    = 1;
    int right  = 78;
    int bottom = 22;
    int width()  const { return right - left - 1; }
    int height() const { return bottom - top - 1; }
};

// ── Game state ──────────────────────────────────────────────────────────────
enum class GameState { Menu, DifficultySelect, Playing, GameOver, Quit };
