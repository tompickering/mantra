#include "win.h"

#include <ncurses.h>

#include <stdlib.h>

Win** wins;

void win_init_all() {
    wins = (Win**) malloc(NWIN * sizeof(Win*));
    for (int i = 0; i < NWIN; ++i) {
        wins[i] = (Win*) malloc(sizeof(Win));
        wins[i]->win = newwin(0, 0, 0, 0);
    }
    wins[WIN_IDX_BOOKMARKS]->draw = draw_win_bookmarks;
    wins[WIN_IDX_PAGES    ]->draw = draw_win_pages    ;
    wins[WIN_IDX_HELPBAR  ]->draw = draw_win_helpbar  ;
}

void win_update(int idx, int x, int y, int r, int c) {
    WINDOW* win = wins[idx]->win;
    wresize(win, r, c);
    mvwin(win, y, x);
}

void draw_windows() {
    for (int i = 0; i < NWIN; ++i)
        wins[i]->draw();
}
