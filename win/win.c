#include "win.h"

#include <ncurses.h>

#include <stdlib.h>
#include <string.h>

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

    wins[WIN_IDX_BOOKMARKS]->can_be_active = true;
    wins[WIN_IDX_PAGES    ]->can_be_active = true;
    wins[WIN_IDX_HELPBAR  ]->can_be_active = true;

    init_pair(WIN_COL_PAIR_NORMAL, COLOR_WHITE, COLOR_BLACK);
    init_pair(WIN_COL_PAIR_ACTIVE, COLOR_GREEN, COLOR_BLACK);
    win_act_idx = WIN_IDX_BOOKMARKS;
}

void win_update(int idx, int x, int y, int r, int c) {
    WINDOW* win = wins[idx]->win;
    wresize(win, r, c);
    mvwin(win, y, x);
}

void win_clear_all() {
    int i, j, r, c;
    WINDOW* win;
    char wipe_char = ' ';
    char* wiper;
    for (i = 0; i < NWIN; ++i) {
        win = wins[i]->win;
        getmaxyx(win, r, c);
        wiper = (char*) malloc((c + 1) * sizeof(char));
        memset(wiper, wipe_char, c);
        wiper[c] = '\0';
        for (j = 0; j < r; ++j)
            mvwprintw(win, j, 0, wiper);
    }
}

/**
 * Find the next Win for which can_be_active is true
 * and make it the currently-active window.
 */
void win_cycle_active() {
    int idx;
    for (idx = (win_act_idx + 1) % NWIN; idx != win_act_idx; idx = ++idx % NWIN) {
        if (wins[idx]->can_be_active) {
            win_act_idx = idx;
            break;
        }
    }
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
