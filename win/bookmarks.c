#include "win.h"

#include <ncurses.h>

#include "../input.h"
#include "../page.h"

void draw_win_bookmarks() {
    Win* win = wins[WIN_IDX_BOOKMARKS];
    win_draw_border(win);
    wrefresh(win->win);
}

void input_win_bookmarks(int ch) {
}
