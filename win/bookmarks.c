#include <ncurses.h>

#include "win.h"

WINDOW* win_bookmarks = NULL;

void draw_win_bookmarks(int x, int y, int w, int h) {
    if (!win_bookmarks) win_init(&win_bookmarks);
    win_setup(win_bookmarks, x, y, w, h);
    box(win_bookmarks, 0, 0);
    wrefresh(win_bookmarks);
}
