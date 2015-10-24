#include "draw.h"

#include <stdbool.h>

#include <ncurses.h>

#include "win/win.h"

float PNL_SF_W = 0.6;
float PNL_SF_H = 0.5;

void get_dims(int* r, int* c) {
    getmaxyx(stdscr, *r, *c);
}

void draw_windows(int r, int c) {
    int h_bk = r / 3;
    int pnl_w = (int) (c * PNL_SF_W);
    int pnl_h = (int) (r * PNL_SF_H);
    int pnl_xoff = (int) (c - pnl_w) / 2;
    int pnl_yoff = (int) (r - pnl_h) / 2;
    win_update(wins[WIN_IDX_BOOKMARKS], 0, 0, h_bk, c);
    win_update(wins[WIN_IDX_PAGES], 0, h_bk, r - h_bk - 1, c);
    win_update(wins[WIN_IDX_HELPBAR], 0, r - 1, 1, c);
    win_update(wins[WIN_IDX_BOOKPNL], pnl_xoff, pnl_yoff, pnl_h, pnl_w);
    win_draw_all();
}

void draw_screen() {
    int rows, cols;
    get_dims(&rows, &cols);
    draw_windows(rows, cols);
}
