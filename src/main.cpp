#include "game.hpp"
#include "renderer.hpp"
#include "types.hpp"
#include <ncurses.h>
#include <locale.h>

int main() {
    // Enable Unicode support
    setlocale(LC_ALL, "");

    Renderer renderer;
    GameState state = GameState::Menu;
    int menu_selection = 0;
    int diff_selection = 1; // Default to Medium

    while (state != GameState::Quit) {
        switch (state) {
            case GameState::Menu: {
                renderer.draw_menu(menu_selection);
                int ch = getch();

                switch (ch) {
                    case 'w': case 'W': case KEY_UP:
                        menu_selection = (menu_selection - 1 + 2) % 2;
                        break;
                    case 's': case 'S': case KEY_DOWN:
                        menu_selection = (menu_selection + 1) % 2;
                        break;
                    case ' ': case '\n': case '\r': case KEY_ENTER:
                        if (menu_selection == 0) {
                            state = GameState::DifficultySelect;
                        } else {
                            state = GameState::Quit;
                        }
                        break;
                    case 'q': case 'Q':
                        state = GameState::Quit;
                        break;
                }
                napms(50);
                break;
            }

            case GameState::DifficultySelect: {
                renderer.draw_difficulty_select(diff_selection);
                int ch = getch();

                switch (ch) {
                    case 'w': case 'W': case KEY_UP:
                        diff_selection = (diff_selection - 1 + 3) % 3;
                        break;
                    case 's': case 'S': case KEY_DOWN:
                        diff_selection = (diff_selection + 1) % 3;
                        break;
                    case ' ': case '\n': case '\r': case KEY_ENTER: {
                        Difficulty diff;
                        switch (diff_selection) {
                            case 0: diff = Difficulty::Easy;   break;
                            case 1: diff = Difficulty::Medium; break;
                            case 2: diff = Difficulty::Hard;   break;
                            default: diff = Difficulty::Medium; break;
                        }
                        Game game(diff, renderer);
                        GameState result = game.run();
                        state = result;
                        menu_selection = 0;
                        break;
                    }
                    case 27: // ESC
                        state = GameState::Menu;
                        break;
                    case 'q': case 'Q':
                        state = GameState::Menu;
                        break;
                }
                napms(50);
                break;
            }

            case GameState::GameOver:
                state = GameState::Menu;
                break;

            default:
                state = GameState::Quit;
                break;
        }
    }

    return 0;
}
