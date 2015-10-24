#include "win.h"

#include <ncurses.h>

#include "../input.h"
#include "../page.h"

void draw_win_bookpnl() {
    Win* win = wins[WIN_IDX_BOOKPNL];
    if (panel_hidden(win->pnl))
        return;
    win_draw_border(win);
    wrefresh(win->win);
}

void input_win_bookpnl(int ch) {
    if (ch == K_RETURN)
        close_panel();
}
