#include <ncurses.h>

#include "win.h"

void draw_win_pages() {
    Win* win = wins[WIN_IDX_PAGES];
    box(win->win, 0, 0);
    wrefresh(win->win);
}
