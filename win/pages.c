#include "win.h"

#include <ncurses.h>

#include "../page.h"

int current_row = 0;

void win_page_show(Win* win) {
    int r = 1;
    int c = 2;
    int col_pair;
    int page_off = 0;
    char sect[2]; sect[1] = '\0';
    Page* page;

    // TODO: Define max. character print limit for name
    for (; r < win->r - 1; ++r) {
        page = &pages[page_off++];
        sect[0] = '0' + page->sect;
        mvwprintw(win->win, r, c, sect);
        mvwprintw(win->win, r, c + 2, page->name);
    }

    col_pair = WIN_COL_PAIR_NORMAL;
    if (win == wins[win_act_idx])
        col_pair = WIN_COL_PAIR_ACTIVE;
    mvwchgat(win->win, current_row + 1, 1, win->c - 2, A_REVERSE, col_pair, NULL);
}

void draw_win_pages() {
    Win* win = wins[WIN_IDX_PAGES];
    win_draw_border(win);
    win_page_show(win);
    wrefresh(win->win);
}
