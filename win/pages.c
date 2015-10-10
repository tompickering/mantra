#include "win.h"

#include <stdlib.h>
#include <string.h>

#include <ncurses.h>

#include "../input.h"
#include "../page.h"

int _current_row = 0;
int _prev_row = 0;
int _page_start = 0;
int _MAX_NAME_LEN = 20;

void win_page_show(Win* win) {
    int r = 1;
    int c = 2;
    int col_pair;
    int name_len;
    int page_off = _page_start;
    char sect[2]; sect[1] = '\0';
    Page* page;
    char* name = (char*) malloc((_MAX_NAME_LEN + 1) * sizeof(char));

    for (; r < win->r - 1; ++r) {
        page = &pages[page_off++];
        sect[0] = '0' + page->sect;
        mvwprintw(win->win, r, c, sect);
        name_len = strlen(page->name);
        strncpy(name, page->name, _MAX_NAME_LEN);
        if (name_len > _MAX_NAME_LEN)
            name[name_len] = '\0';
        else {
            memset(name + name_len, ' ', _MAX_NAME_LEN - name_len);
            name[_MAX_NAME_LEN] = '\0';
        }
        mvwprintw(win->win, r, c + 2, name);
        mvwprintw(win->win, r, c + 3 + _MAX_NAME_LEN, page->desc);
    }

    col_pair = WIN_COL_PAIR_NORMAL;
    if (win == wins[win_act_idx])
        col_pair = WIN_COL_PAIR_ACTIVE;
    mvwchgat(win->win, _current_row + 1, 1, win->c - 2, A_REVERSE, col_pair, NULL);
    if (_current_row != _prev_row) {
        mvwchgat(win->win, _prev_row + 1, 1, win->c - 2, A_NORMAL, WIN_COL_PAIR_NORMAL, NULL);
        _prev_row = _current_row;
    }

    free(name);
}

void draw_win_pages() {
    Win* win = wins[WIN_IDX_PAGES];
    win_page_show(win);
    win_draw_border(win);
    wrefresh(win->win);
}

void _navigate(bool down) {
    Win* win = wins[WIN_IDX_PAGES];
    if (down) {
        if (_current_row + 3 < win->r) {
            ++_current_row;
        } else if (_page_start + win->r - 2 <= NPAGES) {
            ++_page_start;
        }
    } else {
        if (_current_row == 0) {
            if (_page_start > 0)
                --_page_start;
        } else {
            --_current_row;
        }
    }
}

void input_win_pages(int ch) {
    bool down;
    switch (ch) {
        case K_UP:
        case K_DOWN:
            down = (ch == K_UP) ? false : true;
            _navigate(down);
    }
}
