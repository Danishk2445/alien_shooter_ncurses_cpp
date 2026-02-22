#include "renderer.hpp"
#include <string>
#include <cstring>

Renderer::Renderer() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    mousemask(0, nullptr);

    if (has_colors()) {
        start_color();
        use_default_colors();
        init_colors();
    }
}

Renderer::~Renderer() {
    endwin();
}

void Renderer::init_colors() {
    // Basic pairs
    init_pair(CP_CYAN,    COLOR_CYAN,    -1);
    init_pair(CP_MAGENTA, COLOR_MAGENTA, -1);
    init_pair(CP_GREEN,   COLOR_GREEN,   -1);
    init_pair(CP_YELLOW,  COLOR_YELLOW,  -1);
    init_pair(CP_RED,     COLOR_RED,     -1);
    init_pair(CP_WHITE,   COLOR_WHITE,   -1);
    init_pair(CP_BLUE,    COLOR_BLUE,    -1);

    // Neon pairs (same colors, used with A_BOLD for glow)
    init_pair(CP_NEON_PINK,   COLOR_MAGENTA, -1);
    init_pair(CP_NEON_GREEN,  COLOR_GREEN,   -1);
    init_pair(CP_NEON_CYAN,   COLOR_CYAN,    -1);

    // HUD
    init_pair(CP_HUD_BG,       COLOR_WHITE, COLOR_BLUE);
    init_pair(CP_HEALTH_HIGH,  COLOR_GREEN,   -1);
    init_pair(CP_HEALTH_MED,   COLOR_YELLOW,  -1);
    init_pair(CP_HEALTH_LOW,   COLOR_RED,     -1);

    // Menu
    init_pair(CP_TITLE,         COLOR_MAGENTA, -1);
    init_pair(CP_MENU_SELECTED, COLOR_CYAN,    -1);
    init_pair(CP_MENU_NORMAL,   COLOR_WHITE,   -1);

    // Effects
    init_pair(CP_BORDER,       COLOR_CYAN,    -1);
    init_pair(CP_EXPLOSION,    COLOR_YELLOW,  -1);
    init_pair(CP_PICKUP_AMMO,  COLOR_GREEN,   -1);
    init_pair(CP_PICKUP_HEALTH,COLOR_RED,     -1);
    init_pair(CP_SCORE,        COLOR_YELLOW,  -1);
    init_pair(CP_WAVE,         COLOR_MAGENTA, -1);
}

void Renderer::get_game_area(GameArea& area) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    area.left   = 0;
    area.top    = 0;
    area.right  = max_x - 1;
    area.bottom = max_y - 3; // Leave room for HUD at bottom
}

void Renderer::draw_neon_title() {
    const char* title[] = {
        R"(   _   _     ___ ___ _  _   ___  _  _  ___   ___ _____ ___ ___ )",
        R"(  /_\ | |   |_ _| __| \| | / __|| || |/ _ \ / _ \_   _| __| _ \)",
        R"( / _ \| |__  | || _|| .` | \__ \| __ | (_) | (_) || | | _||   /)",
        R"(/_/ \_\____||___|___|_|\_| |___/|_||_|\___/ \___/ |_| |___|_|_\)",
    };

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int start_y = max_y / 2 - 8;

    for (int i = 0; i < 4; i++) {
        int len = (int)strlen(title[i]);
        int start_x = (max_x - len) / 2;
        if (start_x < 0) start_x = 0;

        // Alternate neon colors for each line
        int color = (i % 2 == 0) ? CP_NEON_CYAN : CP_NEON_PINK;
        attron(COLOR_PAIR(color) | A_BOLD);
        mvprintw(start_y + i, start_x, "%s", title[i]);
        attroff(COLOR_PAIR(color) | A_BOLD);
    }

    // Subtitle
    const char* subtitle = ">>> ALIEN INVASION <<<";
    int sub_len = (int)strlen(subtitle);
    attron(COLOR_PAIR(CP_NEON_GREEN) | A_BOLD | A_BLINK);
    mvprintw(start_y + 6, (max_x - sub_len) / 2, "%s", subtitle);
    attroff(COLOR_PAIR(CP_NEON_GREEN) | A_BOLD | A_BLINK);
}

void Renderer::draw_menu(int selected) {
    clear_screen();
    draw_neon_title();

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int center_y = max_y / 2 + 2;

    const char* options[] = {"[ PLAY ]", "[ QUIT ]"};
    for (int i = 0; i < 2; i++) {
        int len = (int)strlen(options[i]);
        int x = (max_x - len) / 2;
        if (i == selected) {
            attron(COLOR_PAIR(CP_MENU_SELECTED) | A_BOLD);
            mvprintw(center_y + i * 2, x, "%s", options[i]);
            // Draw selection arrows
            mvprintw(center_y + i * 2, x - 3, ">>");
            mvprintw(center_y + i * 2, x + len + 1, "<<");
            attroff(COLOR_PAIR(CP_MENU_SELECTED) | A_BOLD);
        } else {
            attron(COLOR_PAIR(CP_MENU_NORMAL));
            mvprintw(center_y + i * 2, x, "%s", options[i]);
            attroff(COLOR_PAIR(CP_MENU_NORMAL));
        }
    }

    // Controls hint
    const char* hint = "Use W/S to navigate, SPACE/ENTER to select";
    attron(COLOR_PAIR(CP_WHITE) | A_DIM);
    mvprintw(max_y - 2, (max_x - (int)strlen(hint)) / 2, "%s", hint);
    attroff(COLOR_PAIR(CP_WHITE) | A_DIM);

    refresh_screen();
}

void Renderer::draw_difficulty_select(int selected) {
    clear_screen();

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int center_y = max_y / 2 - 3;

    const char* header = "=== SELECT DIFFICULTY ===";
    attron(COLOR_PAIR(CP_NEON_PINK) | A_BOLD);
    mvprintw(center_y - 2, (max_x - (int)strlen(header)) / 2, "%s", header);
    attroff(COLOR_PAIR(CP_NEON_PINK) | A_BOLD);

    struct DiffOption {
        const char* label;
        const char* desc;
        int color;
    };

    DiffOption options[] = {
        {"[  EASY  ]",  "Slow aliens, more ammo drops, forgiving",    CP_NEON_GREEN},
        {"[ MEDIUM ]",  "Balanced challenge, moderate AI",            CP_NEON_CYAN},
        {"[  HARD  ]",  "Fast aliens, smart dodging, relentless",     CP_NEON_PINK},
    };

    for (int i = 0; i < 3; i++) {
        int label_len = (int)strlen(options[i].label);
        int x = (max_x - label_len) / 2;
        int y = center_y + i * 3;

        if (i == selected) {
            attron(COLOR_PAIR(options[i].color) | A_BOLD);
            mvprintw(y, x, "%s", options[i].label);
            mvprintw(y, x - 3, ">>");
            mvprintw(y, x + label_len + 1, "<<");
            attroff(COLOR_PAIR(options[i].color) | A_BOLD);

            int desc_len = (int)strlen(options[i].desc);
            attron(COLOR_PAIR(CP_WHITE) | A_DIM);
            mvprintw(y + 1, (max_x - desc_len) / 2, "%s", options[i].desc);
            attroff(COLOR_PAIR(CP_WHITE) | A_DIM);
        } else {
            attron(COLOR_PAIR(CP_MENU_NORMAL));
            mvprintw(y, x, "%s", options[i].label);
            attroff(COLOR_PAIR(CP_MENU_NORMAL));
        }
    }

    const char* hint = "Use W/S to navigate, SPACE/ENTER to select";
    attron(COLOR_PAIR(CP_WHITE) | A_DIM);
    mvprintw(max_y - 2, (max_x - (int)strlen(hint)) / 2, "%s", hint);
    attroff(COLOR_PAIR(CP_WHITE) | A_DIM);

    refresh_screen();
}

void Renderer::draw_game_over(int score, int wave, Difficulty diff) {
    clear_screen();

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int cy = max_y / 2 - 4;

    const char* go_text[] = {
        R"(  ____    _    __  __ _____    _____     _______ ____  )",
        R"( / ___|  / \  |  \/  | ____|  / _ \ \   / / ____|  _ \ )",
        R"(| |  _  / _ \ | |\/| |  _|   | | | \ \ / /|  _| | |_) |)",
        R"(| |_| |/ ___ \| |  | | |___  | |_| |\ V / | |___|  _ < )",
        R"( \____/_/   \_\_|  |_|_____|  \___/  \_/  |_____|_| \_\)",
    };

    for (int i = 0; i < 5; i++) {
        int len = (int)strlen(go_text[i]);
        int x = (max_x - len) / 2;
        if (x < 0) x = 0;
        attron(COLOR_PAIR(CP_RED) | A_BOLD);
        mvprintw(cy + i, x, "%s", go_text[i]);
        attroff(COLOR_PAIR(CP_RED) | A_BOLD);
    }

    auto cfg = get_difficulty_config(diff);

    char score_str[64];
    snprintf(score_str, sizeof(score_str), "FINAL SCORE: %d", score);
    attron(COLOR_PAIR(CP_SCORE) | A_BOLD);
    mvprintw(cy + 7, (max_x - (int)strlen(score_str)) / 2, "%s", score_str);
    attroff(COLOR_PAIR(CP_SCORE) | A_BOLD);

    char wave_str[64];
    snprintf(wave_str, sizeof(wave_str), "WAVE REACHED: %d", wave);
    attron(COLOR_PAIR(CP_WAVE) | A_BOLD);
    mvprintw(cy + 9, (max_x - (int)strlen(wave_str)) / 2, "%s", wave_str);
    attroff(COLOR_PAIR(CP_WAVE) | A_BOLD);

    char diff_str[64];
    snprintf(diff_str, sizeof(diff_str), "DIFFICULTY: %s", cfg.label.c_str());
    attron(COLOR_PAIR(CP_NEON_CYAN) | A_BOLD);
    mvprintw(cy + 11, (max_x - (int)strlen(diff_str)) / 2, "%s", diff_str);
    attroff(COLOR_PAIR(CP_NEON_CYAN) | A_BOLD);

    const char* retry = "Press ENTER to return to menu, Q to quit";
    attron(COLOR_PAIR(CP_WHITE) | A_BLINK);
    mvprintw(cy + 14, (max_x - (int)strlen(retry)) / 2, "%s", retry);
    attroff(COLOR_PAIR(CP_WHITE) | A_BLINK);

    refresh_screen();
}

void Renderer::draw_border(const GameArea& area) {
    attron(COLOR_PAIR(CP_BORDER) | A_BOLD);

    // Top and bottom borders
    for (int x = area.left; x <= area.right; x++) {
        mvaddch(area.top, x, ACS_HLINE);
        mvaddch(area.bottom, x, ACS_HLINE);
    }
    // Left and right borders
    for (int y = area.top; y <= area.bottom; y++) {
        mvaddch(y, area.left, ACS_VLINE);
        mvaddch(y, area.right, ACS_VLINE);
    }
    // Corners
    mvaddch(area.top, area.left, ACS_ULCORNER);
    mvaddch(area.top, area.right, ACS_URCORNER);
    mvaddch(area.bottom, area.left, ACS_LLCORNER);
    mvaddch(area.bottom, area.right, ACS_LRCORNER);

    attroff(COLOR_PAIR(CP_BORDER) | A_BOLD);
}

void Renderer::draw_player(const Player& player, const GameArea& area) {
    if (!player.alive) return;

    // Blink when invincible
    if (player.invincible_timer > 0 && (player.invincible_timer / 3) % 2 == 0)
        return;

    int color = CP_NEON_CYAN;
    chtype glyph;

    switch (player.facing) {
        case Direction::Up:    glyph = ACS_UARROW; break;
        case Direction::Down:  glyph = ACS_DARROW; break;
        case Direction::Left:  glyph = ACS_LARROW; break;
        case Direction::Right: glyph = ACS_RARROW; break;
        default:               glyph = ACS_UARROW; break;
    }

    attron(COLOR_PAIR(color) | A_BOLD);
    mvaddch(player.pos.y, player.pos.x, glyph);
    attroff(COLOR_PAIR(color) | A_BOLD);

    // Draw a small "glow" around player using dim characters
    attron(COLOR_PAIR(CP_CYAN) | A_DIM);
    if (player.pos.x - 1 > area.left)
        mvaddch(player.pos.y, player.pos.x - 1, '.');
    if (player.pos.x + 1 < area.right)
        mvaddch(player.pos.y, player.pos.x + 1, '.');
    attroff(COLOR_PAIR(CP_CYAN) | A_DIM);
}

void Renderer::draw_aliens(const std::vector<Alien>& aliens) {
    // Different alien glyphs
    static const char* glyphs[] = {"W", "M", "X", "@", "#", "V"};
    static const int glyph_count = 6;

    for (const auto& a : aliens) {
        if (!a.alive) continue;

        int color = (a.glyph_index % 2 == 0) ? CP_NEON_PINK : CP_NEON_GREEN;

        attron(COLOR_PAIR(color) | A_BOLD);
        mvprintw(a.pos.y, a.pos.x, "%s", glyphs[a.glyph_index % glyph_count]);
        attroff(COLOR_PAIR(color) | A_BOLD);
    }
}

void Renderer::draw_bullets(const std::vector<Bullet>& bullets) {
    for (const auto& b : bullets) {
        if (!b.alive) continue;

        if (b.owner == Owner::Player) {
            attron(COLOR_PAIR(CP_YELLOW) | A_BOLD);
            char ch = (b.dy != 0) ? '|' : '-';
            mvaddch(b.pos.y, b.pos.x, ch);
            attroff(COLOR_PAIR(CP_YELLOW) | A_BOLD);
        } else {
            attron(COLOR_PAIR(CP_RED) | A_BOLD);
            mvaddch(b.pos.y, b.pos.x, '*');
            attroff(COLOR_PAIR(CP_RED) | A_BOLD);
        }
    }
}

void Renderer::draw_pickups(const std::vector<Pickup>& pickups) {
    for (const auto& p : pickups) {
        if (!p.alive || !p.is_visible()) continue;

        if (p.type == PickupType::Ammo) {
            attron(COLOR_PAIR(CP_PICKUP_AMMO) | A_BOLD);
            mvaddch(p.pos.y, p.pos.x, 'A');
            attroff(COLOR_PAIR(CP_PICKUP_AMMO) | A_BOLD);
        } else {
            attron(COLOR_PAIR(CP_PICKUP_HEALTH) | A_BOLD);
            mvaddch(p.pos.y, p.pos.x, '+');
            attroff(COLOR_PAIR(CP_PICKUP_HEALTH) | A_BOLD);
        }
    }
}

void Renderer::draw_hud(const Player& player, int wave, Difficulty diff, const GameArea& area) {
    int hud_y = area.bottom + 1;
    int max_x = area.right + 1;

    // Clear HUD area
    attron(COLOR_PAIR(CP_WHITE));
    for (int x = 0; x < max_x; x++) {
        mvaddch(hud_y, x, ' ');
        mvaddch(hud_y + 1, x, ' ');
    }
    attroff(COLOR_PAIR(CP_WHITE));

    // ── Row 1: Health bar + Ammo ──
    // Health label
    attron(COLOR_PAIR(CP_WHITE) | A_BOLD);
    mvprintw(hud_y, 1, "HP:");
    attroff(COLOR_PAIR(CP_WHITE) | A_BOLD);

    // Health bar
    int bar_width = 20;
    int filled = (player.health * bar_width) / player.max_health;
    int hp_color = CP_HEALTH_HIGH;
    if (player.health <= 30) hp_color = CP_HEALTH_LOW;
    else if (player.health <= 60) hp_color = CP_HEALTH_MED;

    mvaddch(hud_y, 5, '[');
    attron(COLOR_PAIR(hp_color) | A_BOLD);
    for (int i = 0; i < bar_width; i++) {
        mvaddch(hud_y, 6 + i, (i < filled) ? '=' : ' ');
    }
    attroff(COLOR_PAIR(hp_color) | A_BOLD);
    mvaddch(hud_y, 6 + bar_width, ']');

    // Health number
    char hp_str[16];
    snprintf(hp_str, sizeof(hp_str), "%d%%", player.health);
    attron(COLOR_PAIR(hp_color) | A_BOLD);
    mvprintw(hud_y, 8 + bar_width, "%s", hp_str);
    attroff(COLOR_PAIR(hp_color) | A_BOLD);

    // Ammo
    int ammo_x = 35;
    attron(COLOR_PAIR(CP_NEON_GREEN) | A_BOLD);
    mvprintw(hud_y, ammo_x, "AMMO: %d", player.ammo);
    attroff(COLOR_PAIR(CP_NEON_GREEN) | A_BOLD);

    // ── Row 2: Score + Wave + Difficulty ──
    attron(COLOR_PAIR(CP_SCORE) | A_BOLD);
    mvprintw(hud_y + 1, 1, "SCORE: %d", player.score);
    attroff(COLOR_PAIR(CP_SCORE) | A_BOLD);

    attron(COLOR_PAIR(CP_WAVE) | A_BOLD);
    mvprintw(hud_y + 1, 20, "WAVE: %d", wave);
    attroff(COLOR_PAIR(CP_WAVE) | A_BOLD);

    auto cfg = get_difficulty_config(diff);
    int diff_color = CP_NEON_GREEN;
    if (diff == Difficulty::Medium) diff_color = CP_NEON_CYAN;
    if (diff == Difficulty::Hard)   diff_color = CP_NEON_PINK;

    attron(COLOR_PAIR(diff_color) | A_BOLD);
    mvprintw(hud_y + 1, ammo_x, "[%s]", cfg.label.c_str());
    attroff(COLOR_PAIR(diff_color) | A_BOLD);

    // Controls reminder on the right
    attron(COLOR_PAIR(CP_WHITE) | A_DIM);
    int ctrl_x = max_x - 28;
    if (ctrl_x > ammo_x + 15)
        mvprintw(hud_y + 1, ctrl_x, "ARROWS:Move SPACE:Shoot Q:Quit");
    attroff(COLOR_PAIR(CP_WHITE) | A_DIM);
}

void Renderer::draw_explosions(const std::vector<Vec2>& explosions, int timer) {
    if (timer <= 0) return;

    static const char* frames[] = {"*", "#", "+", "."};
    int frame = (4 - timer);
    if (frame < 0) frame = 0;
    if (frame > 3) frame = 3;

    attron(COLOR_PAIR(CP_EXPLOSION) | A_BOLD);
    for (const auto& e : explosions) {
        mvprintw(e.y, e.x, "%s", frames[frame]);
    }
    attroff(COLOR_PAIR(CP_EXPLOSION) | A_BOLD);
}

void Renderer::clear_screen() {
    erase();
}

void Renderer::refresh_screen() {
    refresh();
}
