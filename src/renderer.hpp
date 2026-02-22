#pragma once

#include "types.hpp"
#include "player.hpp"
#include "alien.hpp"
#include "bullet.hpp"
#include "pickup.hpp"
#include <vector>
#include <ncurses.h>

class Renderer {
public:
    Renderer();
    ~Renderer();

    void init_colors();
    void get_game_area(GameArea& area);

    void draw_menu(int selected);
    void draw_difficulty_select(int selected);
    void draw_game_over(int score, int wave, Difficulty diff);

    void draw_border(const GameArea& area);
    void draw_player(const Player& player, const GameArea& area);
    void draw_aliens(const std::vector<Alien>& aliens);
    void draw_bullets(const std::vector<Bullet>& bullets);
    void draw_pickups(const std::vector<Pickup>& pickups);
    void draw_hud(const Player& player, int wave, Difficulty diff, const GameArea& area);
    void draw_explosions(const std::vector<Vec2>& explosions, int timer);

    void clear_screen();
    void refresh_screen();

private:
    void draw_neon_title();
};
