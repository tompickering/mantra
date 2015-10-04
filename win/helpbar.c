#include "win.h"

#include <ncurses.h>
#include <string.h>

WINDOW* win_helpbar = NULL;

const char helpstr[] = "/-Search   Space-Switch   Enter-Open   jk-Navigate   t-Tag   f-Filter";

void draw_win_helpbar(int x, int y, int r, int c) {
    int helplen, xoff;
    if (!win_helpbar) win_init(&win_helpbar);
    win_setup(win_helpbar, x, y, r, c);
    helplen = strlen(helpstr);
    xoff = (c - helplen) >> 1;
    if (xoff > 0)
        wmove(win_helpbar, 0, xoff);
    wprintw(win_helpbar, "%s", helpstr);
    wrefresh(win_helpbar);
}
