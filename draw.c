#include "draw.h"

#include <ncurses.h>
#include "win/win.h"

void get_dims(int* r, int* c) {
    getmaxyx(stdscr, *r, *c);
}

void draw_windows(int r, int c) {
    int h_bk = r / 3;
    draw_win_bookmarks(0, 0, h_bk, c);
    draw_win_pages(0, h_bk, r - h_bk - 1, c);
    draw_win_helpbar(0, r - 1, 1, c);
}

void draw_screen() {
    int rows, cols;
    get_dims(&rows, &cols);
    draw_windows(rows, cols);
}
