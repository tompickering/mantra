#include <ncurses.h>

#include "win.h"

WINDOW* win_pages = NULL;

void draw_win_pages(int x, int y, int w, int h) {
    if (!win_pages) win_init(&win_pages);
    win_setup(win_pages, x, y, w, h);
    box(win_pages, 0, 0);
    wrefresh(win_pages);
}
