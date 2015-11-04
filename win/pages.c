/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * Mantra: Man page bookmarker                                           *
 * Copyright (C) 2015  Tom Pickering                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "win.h"

#include <stdlib.h>
#include <string.h>

#include <ncurses.h>

#include "../input.h"
#include "../page.h"

int _current_row = 0;
char* _current_name = NULL;
int _current_sect = 0;
int _prev_row = 0;
int _page_start = 0;
int _MAX_NAME_LEN = 20;

void win_page_show(Win* win) {
    int r = 1;
    int c = 2;
    int col_pair;
    int page_off = _page_start;
    Page* page;
    int max_desc_len;
    char* name;
    char* desc;
    char sect[2];

    sect[1] = '\0';

    _MAX_NAME_LEN = win->c / 3;
    max_desc_len = win->c - _MAX_NAME_LEN - 7;
    name = malloc((_MAX_NAME_LEN + 1) * sizeof(char));
    desc = malloc((max_desc_len + 1) * sizeof(char));

    for (; r < win->r - 1; ++r) {
        if (page_off == NPAGES) {
            win_clear_row(win, r);
            continue;
        }
        page = &pages[page_off++];
        sect[0] = '0' + page->sect;
        mvwprintw(win->win, r, c, sect);
        string_clean_buffer(name, page->name, _MAX_NAME_LEN);
        string_clean_buffer(desc, page->desc,  max_desc_len);
        mvwprintw(win->win, r, c + 2, name);
        mvwprintw(win->win, r, c + 3 + _MAX_NAME_LEN, desc);
    }

    col_pair = WIN_COL_PAIR_NORMAL;
    if (win == wins[win_act_idx])
        col_pair = WIN_COL_PAIR_ACTIVE;
    mvwchgat(win->win, _current_row + 1, 1, win->c - 2, A_REVERSE, col_pair, NULL);
    if (_current_row != _prev_row) {
        mvwchgat(win->win, _prev_row + 1, 1, win->c - 2, A_NORMAL, WIN_COL_PAIR_NORMAL, NULL);
        _prev_row = _current_row;
    }

    if (_page_start + _current_row < NPAGES) {
        _current_name = pages[_page_start + _current_row].name;
        _current_sect = pages[_page_start + _current_row].sect;
    } else {
        _current_name = NULL;
        _current_sect = 0;
    }

    free(name);
    free(desc);
}

void draw_win_pages() {
    Win* win = wins[WIN_IDX_PAGES];
    win_page_show(win);
    win_draw_border(win);
    wrefresh(win->win);
}

void _jump_to_end(bool end) {
    Win* win = wins[WIN_IDX_PAGES];
    if (end) {
        _current_row = win->r - 3;
        _page_start = NPAGES - win->r + 2;
    } else {
        _current_row = 0;
        _page_start = 0;
    }
}

void _navigate(bool down) {
    Win* win = wins[WIN_IDX_PAGES];
    if (down) {
        if (_current_row + 3 < win->r) {
            ++_current_row;
        } else if (_page_start + win->r - 2 < NPAGES) {
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

void _page(bool down) {
    Win* win = wins[WIN_IDX_PAGES];
    if (down) {
        if (_page_start + 2 * (win->r - 2) < NPAGES) {
            _page_start += (win->r - 2);
        } else {
            _page_start = NPAGES - (win->r - 2);
        }
    } else {
        _page_start -= (win->r - 2);
        if (_page_start < 0)
            _page_start = 0;
    }
}

void _open_page() {
    if (_current_name != NULL && _current_sect > 0)
        open_page(_current_sect, _current_name, 0);
}

void input_win_pages(int ch) {
    unsigned char sect;
    bool down;
    switch (ch) {
        case K_FWD:
        case K_BACK:
            down = (ch == K_BACK) ? false : true;
            _page(down);
            break;
        case K_UP:
        case K_DOWN:
            down = (ch == K_UP) ? false : true;
            _navigate(down);
            break;
        case K_HOME:
        case K_END:
            down = (ch == K_HOME) ? false : true;
            _jump_to_end(down);
            break;
        case K_OPEN:
            _open_page();
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            sect = ch - '0';
            _page_start = (size_t) (SECT[sect] - pages);
            _current_row = 0;
            break;
    }
}

void update_win_pages() {
    Win* win = wins[WIN_IDX_PAGES];
    int r = win->r;

    if (_current_row > r - 3) {
        _page_start += _current_row - r + 3;
        _current_row = r - 3;
    }
}
