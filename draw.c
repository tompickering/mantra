#include "draw.h"

#include <stdbool.h>

#include <ncurses.h>

#include "win/win.h"

void get_dims(int* r, int* c) {
    getmaxyx(stdscr, *r, *c);
}

void draw_windows(int r, int c) {
    int h_bk = r / 3;
    win_update(wins[WIN_IDX_BOOKMARKS], 0, 0, h_bk, c);
    win_update(wins[WIN_IDX_PAGES], 0, h_bk, r - h_bk - 1, c);
    win_update(wins[WIN_IDX_HELPBAR], 0, r - 1, 1, c);
    win_draw_all();
}

void draw_screen() {
    int rows, cols;
    get_dims(&rows, &cols);
    draw_windows(rows, cols);
}
