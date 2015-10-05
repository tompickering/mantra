#include "win.h"

#include <ncurses.h>

void draw_win_bookmarks() {
    Win* win = wins[WIN_IDX_BOOKMARKS];
    win_draw_border(win);
    wrefresh(win->win);
}
