#include "win.h"
#include <ncurses.h>

void win_init(WINDOW** win) {
    *win = newwin(0, 0, 0, 0);
}

void win_setup(WINDOW* win, int x, int y, int r, int c) {
    wresize(win, r, c);
    mvwin(win, y, x);
}
