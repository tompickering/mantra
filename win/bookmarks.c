#include <ncurses.h>

#include "win.h"

void draw_win_bookmarks() {
    Win* win = wins[WIN_IDX_BOOKMARKS];
    box(win->win, 0, 0);
    wrefresh(win->win);
}
