#include "game.hpp"
#include <cmath>
#include <algorithm>
#include <ncurses.h>

Game::Game(Difficulty diff, Renderer& renderer)
    : renderer_(renderer), difficulty_(diff), config_(get_difficulty_config(diff)),
      rng_(std::random_device{}())
{
    renderer_.get_game_area(area_);

    // Place player at bottom center
    player_ = Player(Vec2{
        area_.left + area_.width() / 2,
        area_.bottom - 2
    });

    // Adjust wave scaling by difficulty
    aliens_to_kill_per_wave_ = 5;
}

GameState Game::run() {
    while (true) {
        // Re-check terminal size each frame
        renderer_.get_game_area(area_);

        int ch = getch();

        // Quit
        if (ch == 'q' || ch == 'Q') {
            return GameState::Menu;
        }

        // Pause
        if (ch == 'p' || ch == 'P') {
            paused_ = !paused_;
        }

        if (!paused_) {
            update(ch);
        }

        render();
        napms(33); // ~30 FPS

        if (!player_.alive) {
            napms(500);
            renderer_.draw_game_over(player_.score, wave_, difficulty_);
            // Wait for input
            nodelay(stdscr, FALSE);
            while (true) {
                int c = getch();
                if (c == '\n' || c == '\r' || c == KEY_ENTER) {
                    nodelay(stdscr, TRUE);
                    return GameState::Menu;
                }
                if (c == 'q' || c == 'Q') {
                    nodelay(stdscr, TRUE);
                    return GameState::Quit;
                }
            }
        }
    }
}

void Game::update(int ch) {
    // Update player
    player_.update(ch, area_);

    // Player shoot
    if ((ch == ' ' || ch == 'j' || ch == 'J') && player_.can_shoot()) {
        bullets_.push_back(player_.shoot());
    }

    // Spawn aliens
    spawn_aliens();

    // Update all entities
    update_bullets();
    update_aliens();
    update_pickups();

    // Collisions
    check_collisions();

    // Explosion timer
    if (explosion_timer_ > 0) {
        explosion_timer_--;
        if (explosion_timer_ <= 0) explosions_.clear();
    }

    // Wave progression
    if (aliens_killed_in_wave_ >= aliens_to_kill_per_wave_) {
        wave_++;
        aliens_killed_in_wave_ = 0;
        aliens_to_kill_per_wave_ = 5 + wave_ * 2;
    }
}

void Game::render() {
    renderer_.clear_screen();
    renderer_.draw_border(area_);
    renderer_.draw_pickups(pickups_);
    renderer_.draw_bullets(bullets_);
    renderer_.draw_aliens(aliens_);
    renderer_.draw_player(player_, area_);
    renderer_.draw_explosions(explosions_, explosion_timer_);
    renderer_.draw_hud(player_, wave_, difficulty_, area_);

    if (paused_) {
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);
        attron(COLOR_PAIR(CP_NEON_CYAN) | A_BOLD | A_BLINK);
        const char* pause_text = "=== PAUSED ===";
        mvprintw(max_y / 2, (max_x - 14) / 2, "%s", pause_text);
        attroff(COLOR_PAIR(CP_NEON_CYAN) | A_BOLD | A_BLINK);
    }

    renderer_.refresh_screen();
}

void Game::spawn_aliens() {
    spawn_timer_++;
    if (spawn_timer_ < config_.spawn_interval) return;
    spawn_timer_ = 0;

    // Don't exceed max aliens
    int alive_count = 0;
    for (const auto& a : aliens_) if (a.alive) alive_count++;
    if (alive_count >= config_.max_aliens) return;

    // Spawn at a random edge
    std::uniform_int_distribution<int> edge(0, 3);
    std::uniform_int_distribution<int> rand_x(area_.left + 2, area_.right - 2);
    std::uniform_int_distribution<int> rand_y(area_.top + 2, area_.bottom - 2);
    std::uniform_int_distribution<int> glyph_dist(0, 5);

    Vec2 spawn_pos;
    int side = edge(rng_);
    switch (side) {
        case 0: spawn_pos = {rand_x(rng_), area_.top + 1};     break; // Top
        case 1: spawn_pos = {rand_x(rng_), area_.bottom - 1};  break; // Bottom
        case 2: spawn_pos = {area_.left + 1, rand_y(rng_)};    break; // Left
        case 3: spawn_pos = {area_.right - 1, rand_y(rng_)};   break; // Right
    }

    // Scale move interval by difficulty (higher mult = slower)
    int move_int = static_cast<int>(3 * config_.enemy_speed_mult);
    if (move_int < 1) move_int = 1;

    // Faster aliens in later waves
    if (wave_ > 3) move_int = std::max(1, move_int - 1);

    aliens_.emplace_back(
        spawn_pos,
        config_.alien_hp,
        move_int,
        config_.alien_shoot_cooldown,
        config_.dodge_chance,
        glyph_dist(rng_)
    );
}

void Game::update_bullets() {
    for (auto& b : bullets_) {
        b.update(area_);
    }
    // Remove dead bullets
    bullets_.erase(
        std::remove_if(bullets_.begin(), bullets_.end(),
            [](const Bullet& b) { return !b.alive; }),
        bullets_.end()
    );
}

void Game::update_aliens() {
    // Collect only player bullets for dodge detection
    std::vector<Bullet> player_bullets;
    for (const auto& b : bullets_) {
        if (b.owner == Owner::Player) player_bullets.push_back(b);
    }

    for (auto& a : aliens_) {
        if (!a.alive) continue;

        a.update(player_.pos, player_bullets, area_, rng_);

        // Alien shooting
        if (a.wants_to_shoot()) {
            bullets_.push_back(a.shoot_at(player_.pos, config_.alien_bullet_speed));
        }
    }

    // Remove dead aliens
    aliens_.erase(
        std::remove_if(aliens_.begin(), aliens_.end(),
            [](const Alien& a) { return !a.alive; }),
        aliens_.end()
    );
}

void Game::update_pickups() {
    for (auto& p : pickups_) {
        p.update();
    }
    pickups_.erase(
        std::remove_if(pickups_.begin(), pickups_.end(),
            [](const Pickup& p) { return !p.alive; }),
        pickups_.end()
    );
}

void Game::check_collisions() {
    // Player bullets → Aliens
    for (auto& b : bullets_) {
        if (!b.alive || b.owner != Owner::Player) continue;

        for (auto& a : aliens_) {
            if (!a.alive) continue;

            if (b.pos.x == a.pos.x && b.pos.y == a.pos.y) {
                b.alive = false;
                a.hp--;
                if (a.hp <= 0) {
                    a.alive = false;
                    player_.score += 100 * config_.score_multiplier;
                    aliens_killed_in_wave_++;

                    // Explosion effect
                    explosions_.push_back(a.pos);
                    explosion_timer_ = 4;

                    // Maybe drop pickup
                    spawn_pickup(a.pos);
                }
                break;
            }
        }
    }

    // Alien bullets → Player
    for (auto& b : bullets_) {
        if (!b.alive || b.owner != Owner::Alien) continue;

        if (b.pos.x == player_.pos.x && b.pos.y == player_.pos.y) {
            b.alive = false;
            player_.take_damage(10);
        }
    }

    // Alien → Player collision (contact damage)
    for (auto& a : aliens_) {
        if (!a.alive) continue;
        if (a.pos.x == player_.pos.x && a.pos.y == player_.pos.y) {
            player_.take_damage(15);
            a.hp--;
            if (a.hp <= 0) {
                a.alive = false;
                aliens_killed_in_wave_++;
                explosions_.push_back(a.pos);
                explosion_timer_ = 4;
            }
        }
    }

    // Player → Pickups
    for (auto& p : pickups_) {
        if (!p.alive) continue;
        if (p.pos.x == player_.pos.x && p.pos.y == player_.pos.y) {
            if (p.type == PickupType::Ammo) {
                player_.collect_ammo(p.get_value());
            } else {
                player_.collect_health(p.get_value());
            }
            p.alive = false;
        }
    }
}

void Game::spawn_pickup(Vec2 pos) {
    std::uniform_real_distribution<float> roll(0.0f, 1.0f);

    // Ammo drop
    if (roll(rng_) < config_.ammo_drop_rate) {
        pickups_.emplace_back(pos, PickupType::Ammo);
    }
    // Health drop (rarer)
    else if (roll(rng_) < config_.health_drop_rate) {
        pickups_.emplace_back(pos, PickupType::Health);
    }
}
