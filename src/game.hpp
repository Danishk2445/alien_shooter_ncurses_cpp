#pragma once

#include "types.hpp"
#include "player.hpp"
#include "alien.hpp"
#include "bullet.hpp"
#include "pickup.hpp"
#include "renderer.hpp"
#include <vector>
#include <random>

class Game {
public:
    Game(Difficulty diff, Renderer& renderer);

    GameState run(); // Returns next state (Menu or Quit)

private:
    void update(int ch);
    void render();

    void spawn_aliens();
    void update_bullets();
    void update_aliens();
    void update_pickups();
    void check_collisions();
    void spawn_pickup(Vec2 pos);

    Renderer&   renderer_;
    Player      player_;
    Difficulty  difficulty_;
    DifficultyConfig config_;
    GameArea    area_;

    std::vector<Alien>  aliens_;
    std::vector<Bullet> bullets_;
    std::vector<Pickup> pickups_;

    // Explosion effects
    std::vector<Vec2> explosions_;
    int explosion_timer_ = 0;

    int wave_        = 1;
    int spawn_timer_ = 0;
    int aliens_killed_in_wave_ = 0;
    int aliens_to_kill_per_wave_ = 5;

    std::mt19937 rng_;

    bool paused_ = false;
};
