#include "win.h"

#include <ncurses.h>

void draw_win_pages() {
    Win* win = wins[WIN_IDX_PAGES];
    win_draw_border(win);
    wrefresh(win->win);
}
