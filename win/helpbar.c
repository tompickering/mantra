#include "win.h"

#include <ncurses.h>
#include <string.h>

const char helpstr[] = "/-Search   Space-Switch   o-Open   jk-Navigate   t-Tag   f-Filter";

void draw_win_helpbar() {
    int rows, cols;
    int helplen, xoff;
    Win* win = wins[WIN_IDX_HELPBAR];
    getmaxyx(win->win, rows, cols);
    helplen = strlen(helpstr);
    xoff = (cols - helplen) >> 1;
    if (xoff > 0)
        wmove(win->win, 0, xoff);
    wprintw(win->win, "%s", helpstr);
    wrefresh(win->win);

}
