#include "win.h"

#include <ncurses.h>

#include <stdlib.h>

const int WIN_COL_PAIR_NORMAL = 0;
const int WIN_COL_PAIR_ACTIVE = 1;

const int WIN_IDX_BOOKMARKS = 0;
const int WIN_IDX_PAGES     = 1;
const int WIN_IDX_HELPBAR   = 2;
const int NWIN              = 3;

Win** wins;
int win_act_idx;

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

    init_pair(WIN_COL_PAIR_NORMAL, COLOR_WHITE, COLOR_BLACK);
    init_pair(WIN_COL_PAIR_ACTIVE, COLOR_GREEN, COLOR_BLACK);
    win_act_idx = WIN_IDX_BOOKMARKS;
}

void win_update(int idx, int x, int y, int r, int c) {
    WINDOW* win = wins[idx]->win;
    wresize(win, r, c);
    mvwin(win, y, x);
}

void win_set_active(int idx) {
    win_act_idx = idx;
}

int win_active() {
    return win_act_idx;
}

void win_draw_border(Win* win) {
    int col_pair = WIN_COL_PAIR_NORMAL;
    if (wins[win_act_idx] == win)
        col_pair = WIN_COL_PAIR_ACTIVE;
    wattron(win->win, COLOR_PAIR(col_pair));
    box(win->win, 0, 0);
    wattroff(win->win, COLOR_PAIR(col_pair));
}

void win_draw_all() {
    int i;
    for (i = 0; i < NWIN; ++i)
        wins[i]->draw();
}
