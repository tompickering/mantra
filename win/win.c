#include "win.h"

#include <ncurses.h>

#include <stdlib.h>

const int WIN_IDX_BOOKMARKS = 0;
const int WIN_IDX_PAGES     = 1;
const int WIN_IDX_HELPBAR   = 2;
const int NWIN              = 3;

Win** wins;

void win_init_all() {
    int i;
    wins = (Win**) malloc(NWIN * sizeof(Win*));
    for (i = 0; i < NWIN; ++i) {
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

void win_draw_border(Win* win) {
    box(win->win, 0, 0);
}

void win_draw_all() {
    int i;
    for (i = 0; i < NWIN; ++i)
        wins[i]->draw();
}
